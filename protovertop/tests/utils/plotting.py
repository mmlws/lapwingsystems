import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path


def plot_profile_elevation(ground_profile, result, output_path):
    """
    Plot ground elevation and optimized road vertical profile

    Args:
        ground_profile: GroundProfile object
        result: OptimizationResult object
        output_path: Path to save figure
    """
    from groundprofile import gp_elevation_at

    fig, ax = plt.subplots(figsize=(12, 6))

    # Plot ground profile (dense interpolation for smooth curve)
    # Use more points for many segments to ensure smooth rendering
    num_eval_points = max(500, result.num_segments * 10)
    s_dense = np.linspace(ground_profile.stations[0], ground_profile.stations[-1], num_eval_points)
    z_ground = gp_elevation_at(ground_profile, s_dense)
    ax.plot(s_dense, z_ground, 'k-', linewidth=2, label='Ground Profile')

    # Plot optimized road profile
    from pwquadratic import pwq_create_from_coeffs, pwq_eval
    pwq_road = pwq_create_from_coeffs(result.stations, result.a0, result.a1, result.a2)
    z_road = pwq_eval(pwq_road, s_dense)

    ax.plot(s_dense, z_road, 'b-', linewidth=2, label='Optimized Road Profile')

    # Mark stations (adjust marker size based on number of stations)
    station_elevs = pwq_eval(pwq_road, result.stations)
    marker_size = max(1, min(6, 60 / result.num_stations))

    ax.plot(result.stations, station_elevs, 'ro', markersize=marker_size, label='Stations', zorder=5)

    # Shade cut/fill regions
    for i in range(result.num_stations):
        s_station = result.stations[i]
        z_ground_i = gp_elevation_at(ground_profile, s_station)
        z_road_i = station_elevs[i]

        # Draw vertical line showing offset
        if result.v_cut[i] > 0.01:
            ax.plot([s_station, s_station], [z_ground_i, z_road_i],
                   'r--', alpha=0.5, linewidth=1)
        if result.v_fill[i] > 0.01:
            ax.plot([s_station, s_station], [z_ground_i, z_road_i],
                   'g--', alpha=0.5, linewidth=1)

    ax.set_xlabel('Station (m)', fontsize=12)
    ax.set_ylabel('Elevation (m)', fontsize=12)
    ax.set_title(f'Vertical Alignment Profile\nTotal Cost: ${result.total_cost:,.0f}',
                 fontsize=14)
    ax.legend(loc='best')
    ax.grid(True, alpha=0.3)

    # Ensure y-axis shows at least 2m range (don't zoom in too much on minor variations)
    ylim = ax.get_ylim()
    y_range = ylim[1] - ylim[0]
    if y_range < 2.0:
        y_center = (ylim[0] + ylim[1]) / 2.0
        ax.set_ylim(y_center - 1.0, y_center + 1.0)

    plt.tight_layout()
    output_path.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(output_path, dpi=150)
    plt.close()


def plot_cutfill_diagram(ground_profile, result, output_path):
    """Plot cut/fill volumes and offset from ground at each station"""
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8), sharex=True)

    stations = result.stations

    # Top plot: Cut/Fill volumes
    width = np.mean(np.diff(stations)) * 0.8 if len(stations) > 1 else 10.0
    ax1.bar(stations, result.v_cut, width=width, color='red', alpha=0.6, label='Cut Volume')
    ax1.bar(stations, -result.v_fill, width=width, color='green', alpha=0.6, label='Fill Volume')
    ax1.axhline(0, color='k', linewidth=0.5)
    ax1.set_ylabel('Volume (m³)', fontsize=12)
    ax1.set_title('Cut/Fill Volumes by Station', fontsize=14)
    ax1.legend()
    ax1.grid(True, alpha=0.3)

    # Bottom plot: Offset from ground (adjust marker size based on number of stations)
    marker_size = max(1, min(6, 60 / result.num_stations))
    ax2.plot(stations, result.u, 'bo-', linewidth=2, markersize=marker_size)
    ax2.axhline(0, color='k', linewidth=1, linestyle='--', label='Ground Level')
    ax2.fill_between(stations, 0, result.u, where=(result.u > 0),
                     color='green', alpha=0.3, label='Fill (above ground)')
    ax2.fill_between(stations, 0, result.u, where=(result.u < 0),
                     color='red', alpha=0.3, label='Cut (below ground)')
    ax2.set_xlabel('Station (m)', fontsize=12)
    ax2.set_ylabel('Offset (m)', fontsize=12)
    ax2.set_title('Vertical Offset from Ground', fontsize=14)
    ax2.legend()
    ax2.grid(True, alpha=0.3)

    # Ensure minimum y-axis ranges (don't zoom in too much on tiny values)
    # Volume plot: at least ±1 m³
    ylim1 = ax1.get_ylim()
    y_range1 = ylim1[1] - ylim1[0]
    if y_range1 < 2.0:  # Total range less than 2 m³
        y_center1 = (ylim1[0] + ylim1[1]) / 2.0
        ax1.set_ylim(y_center1 - 1.0, y_center1 + 1.0)

    # Offset plot: at least ±1 m
    ylim2 = ax2.get_ylim()
    y_range2 = ylim2[1] - ylim2[0]
    if y_range2 < 2.0:  # Total range less than 2 m
        y_center2 = (ylim2[0] + ylim2[1]) / 2.0
        ax2.set_ylim(y_center2 - 1.0, y_center2 + 1.0)

    plt.tight_layout()
    output_path.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(output_path, dpi=150)
    plt.close()


def plot_gradients(result, g_min, g_max, output_path):
    """Plot road gradient along alignment with constraint bounds"""
    from pwquadratic import pwq_create_from_coeffs, pwq_eval

    fig, ax = plt.subplots(figsize=(12, 6))

    # Evaluate elevation at dense points (use more points for many segments)
    num_eval_points = max(500, result.num_segments * 10)
    s_dense = np.linspace(result.stations[0], result.stations[-1], num_eval_points)
    pwq_road = pwq_create_from_coeffs(result.stations, result.a0, result.a1, result.a2)
    z_road = pwq_eval(pwq_road, s_dense)

    # Compute gradient numerically using finite differences
    gradients = np.gradient(z_road, s_dense)

    # Convert to percent
    gradients_pct = gradients * 100

    ax.plot(s_dense, gradients_pct, 'b-', linewidth=2, label='Road Gradient')
    ax.axhline(g_max * 100, color='r', linestyle='--', linewidth=1, label=f'Max Gradient ({g_max*100:.0f}%)')
    ax.axhline(g_min * 100, color='r', linestyle='--', linewidth=1, label=f'Min Gradient ({g_min*100:.0f}%)')
    ax.axhline(0, color='k', linewidth=0.5, alpha=0.5)

    # Shade feasible region
    ax.fill_between(s_dense, g_min * 100, g_max * 100, alpha=0.1, color='green', label='Feasible Region')

    ax.set_xlabel('Station (m)', fontsize=12)
    ax.set_ylabel('Gradient (%)', fontsize=12)
    ax.set_title('Road Gradient Profile', fontsize=14)
    ax.legend(loc='best')
    ax.grid(True, alpha=0.3)

    plt.tight_layout()
    output_path.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(output_path, dpi=150)
    plt.close()


def plot_flow_network(result, output_path):
    """
    Plot material flow network showing stations as vertical slices with nodes and flows

    Each station has:
    - Circular (+) node at top for positive flows
    - Circular (-) node at bottom for negative flows
    - Box in middle showing V_cut (top) and V_fill (bottom)

    Args:
        result: OptimizationResult object
        output_path: Path to save figure
    """
    import matplotlib.patches as mpatches

    num_stations = result.num_stations

    # Layout parameters
    station_spacing = 4.0  # Horizontal spacing between stations
    y_top = 4.0           # Y position of + node
    y_bottom = -4.0       # Y position of - node
    y_box = 0.0           # Y position of center of box
    box_width = 2.0       # Wider to fit V_cut and V_fill side-by-side
    box_height = 1.2
    node_radius = 0.3

    # Figure width scales with number of stations
    fig_width = max(14, num_stations * 3.5)
    fig, ax = plt.subplots(figsize=(fig_width, 10))

    # Draw each station
    for i in range(num_stations):
        x = i * station_spacing

        # Draw + node (positive flows)
        circle_pos = mpatches.Circle((x, y_top), node_radius,
                                     facecolor='lightgreen', edgecolor='black', linewidth=2)
        ax.add_patch(circle_pos)
        ax.text(x, y_top, '+', ha='center', va='center', fontsize=14, fontweight='bold')

        # Draw - node (negative flows)
        circle_neg = mpatches.Circle((x, y_bottom), node_radius,
                                     facecolor='lightcoral', edgecolor='black', linewidth=2)
        ax.add_patch(circle_neg)
        ax.text(x, y_bottom, '-', ha='center', va='center', fontsize=14, fontweight='bold')

        # Draw box for V_cut and V_fill (side-by-side)
        box = mpatches.Rectangle((x - box_width/2, y_box - box_height/2), box_width, box_height,
                                 facecolor='white', edgecolor='black', linewidth=2)
        ax.add_patch(box)

        # Vertical line splitting box (V_cut left, V_fill right)
        ax.plot([x, x], [y_box - box_height/2, y_box + box_height/2], 'k-', linewidth=1)

        # Labels for V_cut (left) and V_fill (right)
        v_cut_val = result.v_cut[i]
        v_fill_val = result.v_fill[i]
        ax.text(x - box_width/4, y_box, f'V_cut\n{v_cut_val:.1f}',
               ha='center', va='center', fontsize=8)
        ax.text(x + box_width/4, y_box, f'V_fill\n{v_fill_val:.1f}',
               ha='center', va='center', fontsize=8)

        # Station label
        ax.text(x, y_bottom - 0.8, f'Station {i}\n{result.stations[i]:.0f}m',
               ha='center', va='top', fontsize=10, fontweight='bold')

    # Draw flows
    arrow_props = dict(arrowstyle='->', lw=1.5, connectionstyle='arc3,rad=0')

    for i in range(num_stations):
        x = i * station_spacing

        # Loading flows: V_cut box (left) -> + node (fl_p) and V_cut box (left) -> - node (fl_n)
        fl_p = result.fl_p[i]
        fl_n = result.fl_n[i]

        # fl_p: V_cut -> + node
        ax.annotate('', xy=(x - 0.2, y_top - node_radius),
                   xytext=(x - box_width/4, y_box + box_height/2),
                   arrowprops={**arrow_props, 'color': 'green'})
        ax.text(x - 0.5, (y_top + y_box)/2, f'fl_p\n{fl_p:.1f}',
               fontsize=7, color='green', ha='right')

        # fl_n: V_cut -> - node
        ax.annotate('', xy=(x - 0.2, y_bottom + node_radius),
                   xytext=(x - box_width/4, y_box - box_height/2),
                   arrowprops={**arrow_props, 'color': 'green'})
        ax.text(x - 0.5, (y_bottom + y_box)/2, f'fl_n\n{fl_n:.1f}',
               fontsize=7, color='green', ha='right')

        # Unloading flows: + node -> V_fill box (right) (fu_p) and - node -> V_fill box (right) (fu_n)
        fu_p = result.fu_p[i]
        fu_n = result.fu_n[i]

        # fu_p: + node -> V_fill
        ax.annotate('', xy=(x + box_width/4, y_box + box_height/2),
                   xytext=(x + 0.2, y_top - node_radius),
                   arrowprops={**arrow_props, 'color': 'blue'})
        ax.text(x + 0.5, (y_top + y_box)/2, f'fu_p\n{fu_p:.1f}',
               fontsize=7, color='blue', ha='left')

        # fu_n: - node -> V_fill
        ax.annotate('', xy=(x + box_width/4, y_box - box_height/2),
                   xytext=(x + 0.2, y_bottom + node_radius),
                   arrowprops={**arrow_props, 'color': 'blue'})
        ax.text(x + 0.5, (y_bottom + y_box)/2, f'fu_n\n{fu_n:.1f}',
               fontsize=7, color='blue', ha='left')

        # Borrow flows (always show, even if zero)
        fb_p = result.fb_p[i]
        fb_n = result.fb_n[i]

        # Only show borrow arrows if non-zero (per user request)
        if fb_p > 0.01:
            ax.annotate('', xy=(x, y_top + node_radius), xytext=(x, y_top + 1.0),
                       arrowprops={**arrow_props, 'color': 'orange'})
            ax.text(x, y_top + 1.2, f'fb_p\n{fb_p:.1f}', ha='center', fontsize=7, color='orange')

        if fb_n > 0.01:
            ax.annotate('', xy=(x, y_bottom - node_radius), xytext=(x, y_bottom - 1.0),
                       arrowprops={**arrow_props, 'color': 'orange'})
            ax.text(x, y_bottom - 1.2, f'fb_n\n{fb_n:.1f}', ha='center', fontsize=7, color='orange')

        # Waste flows (only show if non-zero, per user request)
        fw_p = result.fw_p[i]
        fw_n = result.fw_n[i]

        if fw_p > 0.01:
            ax.annotate('', xy=(x + 0.6, y_top + 0.7), xytext=(x, y_top + node_radius),
                       arrowprops={**arrow_props, 'color': 'red'})
            ax.text(x + 0.8, y_top + 0.8, f'fw_p\n{fw_p:.1f}', ha='left', fontsize=7, color='red')

        if fw_n > 0.01:
            ax.annotate('', xy=(x + 0.6, y_bottom - 0.7), xytext=(x, y_bottom - node_radius),
                       arrowprops={**arrow_props, 'color': 'red'})
            ax.text(x + 0.8, y_bottom - 0.8, f'fw_n\n{fw_n:.1f}', ha='left', fontsize=7, color='red')

    # Transit flows between stations (always show, even if zero)
    for i in range(num_stations - 1):
        x1 = i * station_spacing
        x2 = (i + 1) * station_spacing

        # Positive transit flow (left to right)
        ft_p = result.ft_p[i]
        ax.annotate('', xy=(x2 - node_radius, y_top), xytext=(x1 + node_radius, y_top),
                   arrowprops={**arrow_props, 'color': 'purple', 'lw': 2})
        ax.text((x1 + x2)/2, y_top + 0.5, f'ft_p[{i}]\n{ft_p:.1f}', ha='center', fontsize=8,
               color='purple', fontweight='bold')

        # Negative transit flow (right to left)
        ft_n = result.ft_n[i]
        ax.annotate('', xy=(x1 + node_radius, y_bottom), xytext=(x2 - node_radius, y_bottom),
                   arrowprops={**arrow_props, 'color': 'brown', 'lw': 2})
        ax.text((x1 + x2)/2, y_bottom - 0.5, f'ft_n[{i}]\n{ft_n:.1f}', ha='center', fontsize=8,
               color='brown', fontweight='bold')

    # Set axis limits and appearance
    ax.set_xlim(-2, (num_stations - 1) * station_spacing + 2)
    ax.set_ylim(y_bottom - 2.5, y_top + 2.5)
    ax.set_aspect('equal')
    ax.axis('off')

    # Add centered legend at top (no title to avoid overlap)
    legend_elements = [
        mpatches.Patch(color='green', label='Loading fl (V_cut → nodes)'),
        mpatches.Patch(color='blue', label='Unloading fu (nodes → V_fill)'),
        mpatches.Patch(color='purple', label='Transit ft_p (→)'),
        mpatches.Patch(color='brown', label='Transit ft_n (←)'),
        mpatches.Patch(color='orange', label='Borrow fb (external → nodes)'),
        mpatches.Patch(color='red', label='Waste fw (nodes → external)')
    ]
    ax.legend(handles=legend_elements, loc='upper center', bbox_to_anchor=(0.5, 1.02),
             fontsize=10, ncol=3, frameon=True, fancybox=True, shadow=True)

    plt.tight_layout()
    output_path.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(output_path, dpi=150, bbox_inches='tight')
    plt.close()


def plot_all_results(ground_profile, result, output_dir, test_name):
    """
    Generate all plots for a test case

    Args:
        ground_profile: GroundProfile object
        result: OptimizationResult object
        output_dir: Base output directory
        test_name: Name of test (used for filenames)
    """
    plot_profile_elevation(ground_profile, result,
                          output_dir / "profiles" / f"{test_name}_profile.png")
    plot_cutfill_diagram(ground_profile, result,
                        output_dir / "cutfill" / f"{test_name}_cutfill.png")
    plot_gradients(result, -0.1, 0.1,
                  output_dir / "gradients" / f"{test_name}_gradients.png")
    plot_flow_network(result,
                     output_dir / "flows" / f"{test_name}_flows.png")
