# CLAUDE.md

## Project Overview

Python implementation of vertical road alignment optimization using **convex quadratic programming**. Combines:
- **B-spline representation** from 2011 Van Loock et al. paper (single knot vector, derivative coefficients as variables)
- **Cost optimization objective** from 2025 Sadhukhan et al. paper (minimize earthwork + hauling + loading costs)

**Goal**: Find optimal road elevation profile that minimizes total construction cost while satisfying safety and design constraints.

## Key Innovation: Direct Derivative Variables (from 2011 Paper)

Use spline coefficients **AND their derivative coefficients** as direct optimization variables:

**Decision variables**:
```
c = [c⁽⁰⁾₋ₖ, ..., c⁽⁰⁾_g,      # Spline coefficients (degree k)
     c⁽¹⁾₋ₖ₊₁, ..., c⁽¹⁾_g,    # 1st derivative coefficients (degree k-1)
     c⁽²⁾₋ₖ₊₂, ..., c⁽²⁾_g,    # 2nd derivative coefficients (degree k-2)
     u₁, ..., uₙ,              # Offsets from ground at each section
     V⁺₁, ..., V⁺ₙ,            # Cut volumes
     V⁻₁, ..., V⁻ₙ]            # Fill volumes
```

**Benefits**:
1. **Direct gradient constraints**: `G_min ≤ c⁽¹⁾ᵢ ≤ G_max` instead of evaluating s'(x) everywhere
2. **No inter-segment continuity constraints**: Single knot vector ensures automatic C^(k-1) continuity
3. **Simpler curvature limits**: Bounds on `c⁽²⁾` for passenger comfort
4. **Linear derivative relationships**: Equation (9) from 2011 paper becomes simple equality constraints

## Optimization Problem

### Objective Function (2025 Paper Equation 2, simplified)

```
minimize  Σ (p + y)·V⁺ⱼ + Σ q·V⁻ⱼ + Σ c·d·flow

where:
  p = excavation cost per m³
  y = loading cost per m³
  q = embankment cost per m³
  c = haul cost per m³ per meter
  d = haul distance
  V⁺ = cut volume
  V⁻ = fill volume
```

### Constraints

**1. Gap Constraints** (2025 paper equation 6):
```
P(xⱼ) - Zⱼ = uⱼ

where:
  P(xⱼ) = Σ c⁽⁰⁾ᵢ Bᵢ,ₖ₊₁(xⱼ)  (road elevation)
  Zⱼ = ground elevation at section j
  uⱼ = offset (negative = cut, positive = fill)
```

**2. Grade Constraints** (2025 paper equations 8-9):
```
G_min ≤ c⁽¹⁾ᵢ ≤ G_max    for all i

DIRECT bounds on derivative coefficients!
No need to evaluate P'(x) at multiple points.
```

**3. Volume Constraints** (2025 paper equations 40-41, CUVA):
```
V⁺ⱼ ≥ χ₁·u²ⱼ + χ₂·uⱼ + χ₃    for uⱼ ≤ 0  (cut)
V⁻ⱼ ≥ χ₁·u²ⱼ + χ₂·uⱼ + χ₃    for uⱼ ≥ 0  (fill)

where χ coefficients approxim trapezoidal cross-section volume.
Must have χ₁ > 0 for convexity.
```

**4. Derivative Relationships** (2011 paper equation 9):
```
c⁽ⁱ⁾ⱼ = (c⁽ⁱ⁻¹⁾ⱼ - c⁽ⁱ⁻¹⁾ⱼ₋₁) / (λⱼ₊ₖ₊₁₋ᵢ - λⱼ)

Rewritten as linear equality: C · c = 0
```

**5. Flow Conservation** (2025 paper equations 10-11, simplified for single road):
```
Material balance at each section:
  inflow + cut = outflow + fill
```

### B-Spline Knot Vector (2011 Paper Approach)

**Single knot vector** for entire road alignment:
- Provide many candidate knots (500-1000): λ₁, ..., λ_g
- Boundary padding (equations 5-6 from 2011 paper):
  ```
  λ₋ₖ = ... = λ₀ = x_min (road start)
  λ_g₊₁ = ... = λ_g₊ₖ₊₁ = x_max (road end)
  ```
- **No manual segment management**
- Continuity is automatic by B-spline properties (equation 2 from 2011 paper)
- Most knots become inactive (via regularization if needed)

## Development Commands

**Setup:**
```bash
uv sync
```

**Run optimization:**
```bash
python main.py --ground data/ground_profile.csv --output results/
```

**Run tests:**
```bash
pytest tests/ -v
```

**Run single test:**
```bash
pytest tests/test_qp_formulation.py::test_derivative_constraints -v
```

## Architecture

### Core Components

#### 1. `qbspline.py` - B-Spline Foundation (existing)

Current implementation provides:
- Cox-de Boor basis functions (N0, N1, N2)
- Knot vector management
- Derivative evaluation
- Active basis tracking

**Extensions needed**:
```python
class RoadAlignmentSpline(QuadraticBSpline):
    def build_derivative_constraint_matrix(self):
        """
        Build C matrix encoding equation (9) from 2011 paper:
        c⁽ⁱ⁾ⱼ = (c⁽ⁱ⁻¹⁾ⱼ - c⁽ⁱ⁻¹⁾ⱼ₋₁) / (λⱼ₊ₖ₊₁₋ᵢ - λⱼ)

        Returns: sparse matrix C such that C·c = 0
        """

    def evaluate_at_stations(self, stations):
        """
        Evaluate spline at multiple station positions efficiently.
        Uses active_basis() for sparse computation.

        Returns: elevations at each station
        """
```

#### 2. `ground_profile.py` - Terrain and Cross-Section Data

```python
class GroundProfile:
    """Ground elevation profile along road centerline"""
    stations: np.ndarray         # Station positions (m)
    elevations: np.ndarray       # Ground elevation Z(x)
    cross_sections: List[CrossSection]

class CrossSection:
    """Cross-sectional geometry at a station"""
    station: float
    center_elevation: float
    width: float                 # Road width W (m)
    left_slope_angle: float      # α (radians)
    right_slope_angle: float     # β (radians)
    section_length: float        # Distance to next station
```

#### 3. `volume_approximation.py` - Convex Volume Constraints

Fit quadratic approximation to trapezoidal cross-section volume (2025 paper Section 4.3):

```python
def compute_volume_coefficients(cross_section: CrossSection,
                                offset_range: np.ndarray):
    """
    Compute χ₁, χ₂, χ₃ for volume approximation V ≥ χ₁u² + χ₂u + χ₃

    True volume with side slopes (trapezoid):
    V(u) = W·L·u + 0.5·L·u²·(1/tan(α) + 1/tan(β))

    Fit using least-squares over offset samples.
    Check R² ≥ 0.95 for convexity (2025 paper Section 4.4).
    If R² < 0.95, use linear approximation instead.

    Returns:
        chi_cut: (χ₁, χ₂, χ₃) for cut sections
        chi_fill: (χ₁, χ₂, χ₃) for fill sections
        r_squared: fit quality
    """
```

#### 4. `qp_formulation.py` - Build Optimization Problem for ALGLIB

```python
class RoadAlignmentQP:
    """
    Formulate road alignment as convex QP for ALGLIB minqp solver.

    Standard QP form:
    min  0.5·x^T·Q·x + c^T·x
    s.t. A_eq·x = b_eq
         A_ineq·x ≤ b_ineq
         lb ≤ x ≤ ub
    """

    def __init__(self, ground: GroundProfile, costs: CostParams,
                 config: RoadConfig):
        self.ground = ground
        self.costs = costs
        self.config = config

    def build_decision_vector(self):
        """
        x = [c⁽⁰⁾, c⁽¹⁾, c⁽²⁾, u, V⁺, V⁻]

        Returns: variable indices mapping
        """

    def build_objective(self):
        """
        Objective: (p + y)·ΣV⁺ + q·ΣV⁻ + haul_costs

        For MVP (single haul, adjacent sections only):
        Haul cost can be linearized in objective.

        Returns: Q matrix (mostly zero), c vector
        """

    def build_derivative_constraints(self, spline):
        """
        C·c = 0  (equation 9 from 2011 paper)

        Returns: A_eq rows, b_eq values
        """

    def build_gap_constraints(self, spline):
        """
        P(xⱼ) - Zⱼ = uⱼ

        where P(xⱼ) = Σ c⁽⁰⁾ᵢ Bᵢ,ₖ₊₁(xⱼ)

        Returns: A_eq rows, b_eq values
        """

    def build_grade_constraints(self):
        """
        G_min ≤ c⁽¹⁾ᵢ ≤ G_max

        Returns: lb, ub for c⁽¹⁾ variables
        """

    def build_volume_constraints(self, volume_coeffs):
        """
        V⁺ⱼ ≥ χ₁u²ⱼ + χ₂uⱼ + χ₃  (cut)
        V⁻ⱼ ≥ χ₁u²ⱼ + χ₂uⱼ + χ₃  (fill)

        Quadratic constraints - need to convert for ALGLIB.
        Can linearize or use QCQP capabilities if available.

        Returns: constraint representation
        """

    def build_flow_constraints(self):
        """
        Material balance (simplified single-road version):

        For MVP: material moves only to adjacent sections.

        Returns: A_eq rows for flow conservation
        """
```

#### 5. `alglib_solver.py` - ALGLIB Interface

```python
import alglib

def solve_road_alignment_qp(Q, c, A_eq, b_eq, A_ineq, b_ineq, lb, ub):
    """
    Solve using ALGLIB's minqp module.

    ALGLIB location: D:/repos/alglib/4.06/native-core-wrappers
    (system-specific path)
    """
    n = len(c)

    # Create QP state
    state = alglib.minqpcreate(n)

    # Set quadratic term
    alglib.minqpsetquadraticterm(state, Q)

    # Set linear term
    alglib.minqpsetlinearterm(state, c)

    # Set linear constraints
    if A_eq.size > 0:
        alglib.minqpsetlc(state, A_eq, b_eq)
    if A_ineq.size > 0:
        alglib.minqpsetlc(state, A_ineq, b_ineq, append=True)

    # Set bounds
    alglib.minqpsetbc(state, lb, ub)

    # Solve
    alglib.minqpsetalgoquickqp(state, 0.0, 0.0, 0.0, 0, True)
    alglib.minqpoptimize(state)

    # Extract solution
    x, rep = alglib.minqpresults(state)

    return x, rep
```

#### 6. `visualization.py` - Results Plotting

```python
def plot_road_alignment(solution, ground):
    """
    Four-panel visualization:

    1. Profile view: Ground + Road elevation with cut/fill shading
    2. Gradient: Road slope s'(x) with grade limit bounds
    3. Curvature: s''(x) for comfort analysis
    4. Volumes: Bar chart of cut (red) and fill (blue) at each section
    """
    fig, axes = plt.subplots(4, 1, figsize=(14, 10))

    # Panel 1: Elevation
    axes[0].plot(ground.stations, ground.elevations, label='Ground')
    axes[0].plot(solution.stations, solution.road_elevations, label='Road')
    axes[0].fill_between(...)  # Cut/fill regions

    # Panel 2: Gradient
    axes[1].plot(solution.stations, solution.gradients)
    axes[1].axhline(config.min_grade, color='r', linestyle='--')
    axes[1].axhline(config.max_grade, color='r', linestyle='--')

    # Panel 3: Curvature
    axes[2].plot(solution.stations, solution.curvatures)

    # Panel 4: Volumes
    axes[3].bar(solution.stations, solution.cut_volumes, color='red', alpha=0.5)
    axes[3].bar(solution.stations, -solution.fill_volumes, color='blue', alpha=0.5)
```

## Project Structure

```
protovertop/
├── qbspline.py              # B-spline (existing - extend for constraint matrix)
├── ground_profile.py        # Ground data and cross-sections
├── volume_approximation.py  # χ coefficient fitting
├── qp_formulation.py        # Build QP matrices for ALGLIB
├── alglib_solver.py         # ALGLIB minqp interface
├── visualization.py         # Plot results
├── main.py                  # Pipeline
├── pyproject.toml           # UV dependencies
├── uv.lock
├── tests/
│   ├── test_derivative_constraints.py
│   ├── test_volume_approx.py
│   ├── test_qp_formulation.py
│   └── test_integration.py
├── data/
│   └── sample_ground.csv
└── papers/
    ├── 2011- Convex B-Splines.pdf
    └── 2025 - Unified vertical alignment.pdf
```

## Key Design Patterns

### 1. Single Knot Vector (2011 Paper)

**Traditional approach** (2025 UVA model):
- Multiple spline segments
- Explicit continuity constraints between segments
- Complex variable indexing

**Our approach** (2011 paper):
- One knot vector for entire alignment
- Automatic C^(k-1) continuity (property of B-splines)
- Simpler: fewer constraints, cleaner structure

### 2. Derivative Coefficients as Variables

**Why this matters**:

Without direct derivative variables:
```python
# Complex: Evaluate derivative at many points
for x in sample_points:
    constraint: G_min ≤ spline.evald(x) ≤ G_max
# Requires many constraints, numerical evaluation
```

With derivative coefficient variables:
```python
# Simple: Direct bounds
for i in range(num_first_deriv_coeffs):
    lb[c1_idx + i] = G_min
    ub[c1_idx + i] = G_max
# One bound per coefficient, linear algebra only
```

### 3. Volume Convexification (2025 Paper Section 4.3)

Side slopes → trapezoid → quadratic volume:
```
V(u) = A·u + B·u²

where:
  A = width × section_length
  B = 0.5 × section_length × (1/tan(α) + 1/tan(β))
```

Approximate with convex quadratic:
```
V ≥ χ₁u² + χ₂u + χ₃
```

Pre-compute χ via least-squares → becomes simple QP constraint.

## Dependencies

- **numpy** (≥2.3.5): Numerical arrays
- **alglib** (local build): QP solver at `D:/repos/alglib/4.06/native-core-wrappers`
  - **System-specific path** - update in `pyproject.toml` for your machine
- **scipy** (optional): Sparse matrices
- **matplotlib** (optional): Plotting

## Common Development Tasks

### Add Curvature Constraints

For passenger comfort, limit maximum curvature:

```python
# In qp_formulation.py:
def build_curvature_constraints(self, K_max=0.01):
    """
    Maximum curvature for comfort: |κ| ≤ K_max

    For small gradients, κ ≈ s''(x)
    So: -K_max ≤ c⁽²⁾ᵢ ≤ K_max

    DIRECT bounds on second derivative coefficients!
    """
    # Just set bounds on c⁽²⁾ variables
    lb[c2_start:c2_end] = -K_max
    ub[c2_start:c2_end] = K_max
```

### Debug Infeasible Solutions

1. **Check derivative constraint matrix**:
   ```python
   C = spline.build_derivative_constraint_matrix()
   rank = np.linalg.matrix_rank(C)
   # Should equal number of derivative relationships
   ```

2. **Verify volume coefficients**:
   ```python
   chi1, chi2, chi3 = volume_coeffs[section_idx]
   assert chi1 > 0, "Not convex! Use linear fallback."
   ```

3. **Check grade bounds compatibility**:
   ```python
   # Can ground be followed with grade limits?
   ground_slope = np.diff(ground.elevations) / np.diff(ground.stations)
   if np.any(ground_slope < G_min) or np.any(ground_slope > G_max):
       warnings.warn("Ground slope exceeds grade limits - infeasible!")
   ```

### Testing New Ground Profiles

```bash
# Create CSV: station,elevation,width,left_slope,right_slope
python main.py --ground data/my_road.csv --output results/my_road/
```

## Mathematical Notation Map

**2011 Paper → Code**:
- `cᵢ⁽ᵛ⁾` → `cv[i]` (ν-th derivative coefficient)
- `Bᵢ,ₗ(x)` → `spline.N(i, l, x)`
- `λᵢ` → `knots[i]`
- `s⁽ᵛ⁾(x)` → `spline.eval_deriv(x, order=v)`

**2025 Paper → Code**:
- `uᵢ,ⱼ` → `offsets[j]`
- `V⁺ᵢ,ⱼ` → `volumes_cut[j]`
- `V⁻ᵢ,ⱼ` → `volumes_fill[j]`
- `Zᵢ,ⱼ` → `ground.elevations[j]`
- `Pᵢ,ᵍ(s)` → `spline.eval(s)`
- `GL, GU` → `config.min_grade, config.max_grade`

## Performance Characteristics

- **Problem size**:
  - 1000 candidate knots
  - 100-200 ground sections
  - ~4000-6000 total variables (spline coeffs + derivatives + volumes + offsets)
- **Solve time**: < 10 seconds (convex QP, polynomial complexity)
- **Memory**: Dominated by constraint matrices (sparse storage recommended for large problems)

## MVP Scope

**Included**:
- Single road (no intersections)
- Single material type
- Single haul path (adjacent sections only)
- Quadratic B-splines (k=2)
- Direct derivative variables (2011 approach)
- Cost minimization (2025 objective)
- Convex volume constraints (2025 CUVA)

**Future Extensions**:
- Multiple materials (separate V⁺, V⁻ per material)
- Road networks (intersection constraints from 2025 paper Section 3)
- Multiple haul paths (equipment selection)
- Borrow/waste pits

## Troubleshooting

**Issue**: Grade constraints violated in solution
- Check that grade bounds are applied to c⁽¹⁾ variables correctly
- Verify derivative constraint matrix C is properly constructed
- Inspect knot spacing (too sparse → can't represent needed curvature)

**Issue**: Poor volume approximation (high MAPE)
- Increase offset sample density in least-squares fit
- Check R² values - if < 0.95, should use linear fallback
- Unusual cross-sections (varying width) may need more careful fitting

**Issue**: Solution has cut where should be fill (or vice versa)
- Check ground elevation data sign conventions
- Verify offset sign: negative = road below ground = cut

**Issue**: ALGLIB solver fails
- Verify path in `pyproject.toml` matches installation
- Check matrix dimensions match (common indexing error)
- Test with smaller problem first (10 sections, 50 knots)

## Validation Approach

1. **Synthetic tests**:
   - Flat ground → straight road at ground level
   - Linear slope within grade limits → match ground exactly
   - Step function → smooth transition with minimal earthwork

2. **Known solutions**:
   - Compare with 2025 paper results (Table 5-6 MAPE values)
   - Simple cases with analytical solutions

3. **Constraint verification**:
   - All sections: verify V⁺ ≥ 0, V⁻ ≥ 0
   - Check flow balance: Σ(cut) ≈ Σ(fill) (within haul capacity)
   - Verify C·c = 0 (derivative relationships)

## References

**2011 Van Loock et al.**: B-spline framework with:
- Single knot vector (Section 2.1)
- Derivative coefficients as variables (equation 8-9)
- Derivative relationship constraints (equation 9)

**2025 Sadhukhan et al.**: Road alignment application with:
- Cost objective (Section 3.3, equation 2)
- Volume constraints (Section 4.2-4.3, equations 40-41)
- Gap, grade, flow constraints (Sections 3.4)

**Key synthesis**: 2011's B-spline representation + 2025's road cost optimization
