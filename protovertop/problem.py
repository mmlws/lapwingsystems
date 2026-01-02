from dataclasses import dataclass


@dataclass
class OptimisationProblem:
    # Earthwork costs
    cost_cut: float = 12.0  # Excavation cost ($/m³)
    cost_fill: float = 10.0  # Fill/embankment cost ($/m³)
    cost_load: float = 5.0  # Loading cost ($/m³)
    cost_haul: float = 5.0  # Hauling cost per meter ($/m)
    cost_borrow: float = 15.0  # Borrow pit material cost ($/m³)
    cost_waste: float = 8.0  # Waste disposal cost ($/m³)

    # Vertical offset bounds
    min_offset: float = -4.0  # Maximum cut depth (m, negative)
    max_offset: float = 4.0  # Maximum fill height (m, positive)

    # Gradient constraints
    g_min: float = -0.1  # Minimum gradient (10% downgrade, dimensionless)
    g_max: float = 0.1  # Maximum gradient (10% upgrade, dimensionless)

    # Vertical curvature constraints (K-values from AASHTO)
    k_min_crest: float = 5.0  # Minimum K-value for crest curves (m/%)
    k_min_sag: float = 5.0  # Minimum K-value for sag curves (m/%)

    # Borrow/waste pit capacities
    borrow_capacity: float = 0.0  # Maximum borrow pit usage (m³)
    waste_capacity: float = 0.0  # Maximum waste pit capacity (m³)

    # Cross-section sampling for area fitting
    num_samples: int = 20  # Number of offset samples for area fitting

    def __post_init__(self):
        """Validate parameters after initialization"""
        assert self.cost_cut >= 0, "cost_cut must be non-negative"
        assert self.cost_fill >= 0, "cost_fill must be non-negative"
        assert self.cost_load >= 0, "cost_load must be non-negative"
        assert self.cost_haul >= 0, "cost_haul must be non-negative"
        assert self.cost_borrow >= 0, "cost_borrow must be non-negative"
        assert self.cost_waste >= 0, "cost_waste must be non-negative"

        assert self.min_offset < self.max_offset, "min_offset must be less than max_offset"
        assert self.min_offset <= 0, "min_offset should be negative or zero (cut depth)"
        assert self.max_offset >= 0, "max_offset should be positive or zero (fill height)"

        assert self.g_min < self.g_max, "g_min must be less than g_max"
        assert -1.0 <= self.g_min <= 0.0, "g_min should be in range [-1.0, 0.0]"
        assert 0.0 <= self.g_max <= 1.0, "g_max should be in range [0.0, 1.0]"

        assert self.k_min_crest > 0, "k_min_crest must be positive"
        assert self.k_min_sag > 0, "k_min_sag must be positive"

        assert self.borrow_capacity >= 0, "borrow_capacity must be non-negative"
        assert self.waste_capacity >= 0, "waste_capacity must be non-negative"

        assert self.num_samples >= 10, "num_samples should be at least 10 for good area fit"

    @classmethod
    def default(cls):
        """Default problem parameters (moderate constraints)"""
        return cls()

    @classmethod
    def conservative(cls):
        """Conservative parameters for safe, comfortable design"""
        return cls(
            g_min=-0.06,  # ±6% grades
            g_max=0.06,
            k_min_crest=20.0,  # Gentle curves (higher speed)
            k_min_sag=20.0,
            min_offset=-3.0,  # Limited cut/fill
            max_offset=3.0
        )

    @classmethod
    def aggressive(cls):
        """Aggressive parameters allowing steeper grades and tighter curves"""
        return cls(
            g_min=-0.15,  # ±15% grades (steep)
            g_max=0.15,
            k_min_crest=3.0,  # Tight curves (low speed)
            k_min_sag=3.0,
            min_offset=-6.0,  # Deep cut/high fill allowed
            max_offset=6.0
        )

    @classmethod
    def highway(cls):
        """Highway design standards (AASHTO for 110 km/h)"""
        return cls(
            cost_cut=15.0,  # Higher costs for highway
            cost_fill=12.0,
            cost_haul=8.0,
            g_min=-0.05,  # ±5% grades
            g_max=0.05,
            k_min_crest=84.0,  # AASHTO K for 110 km/h
            k_min_sag=55.0,
            min_offset=-10.0,  # Large cuts/fills acceptable
            max_offset=10.0
        )

    @classmethod
    def local_road(cls):
        """Local road design standards (50 km/h)"""
        return cls(
            cost_cut=10.0,  # Lower costs for local roads
            cost_fill=8.0,
            cost_haul=4.0,
            g_min=-0.12,  # ±12% grades acceptable
            g_max=0.12,
            k_min_crest=7.0,  # AASHTO K for 50 km/h
            k_min_sag=9.0,
            min_offset=-5.0,
            max_offset=5.0
        )
