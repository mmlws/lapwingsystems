# Implementation Plan: Road Alignment MVP

Step-by-step guide for implementing a basic single-road, single-material vertical alignment optimizer using quadratic B-splines and ALGLIB.

## Overview

Build incrementally, testing at each step. Each checkpoint should produce working, testable code before moving forward.

---

## Phase 1: B-Spline Foundation (Week 1)

### Step 1.1: Extend QuadraticBSpline for Derivative Constraints

**Goal**: Generate the C matrix that relates derivative coefficients (equation 9 from 2011 paper).

**Implementation**:
```python
# In qbspline.py, add method:

def build_derivative_constraint_matrix(self):
    """
    Build sparse matrix C such that C·c = 0 enforces:

    c⁽ⁱ⁾ⱼ = (c⁽ⁱ⁻¹⁾ⱼ - c⁽ⁱ⁻¹⁾ⱼ₋₁) / (λⱼ₊ₖ₊₁₋ᵢ - λⱼ)

    For quadratic splines (k=2):
    - First derivative: c⁽¹⁾ⱼ·(λⱼ₊₂ - λⱼ) - c⁽⁰⁾ⱼ + c⁽⁰⁾ⱼ₋₁ = 0
    - Second derivative: c⁽²⁾ⱼ·(λⱼ₊₁ - λⱼ) - c⁽¹⁾ⱼ + c⁽¹⁾ⱼ₋₁ = 0

    Returns: scipy.sparse matrix, shape (num_derivative_constraints, num_total_coeffs)
    """
    pass
```

**Test Plan**:
```python
# tests/test_derivative_constraints.py

def test_derivative_matrix_shape():
    """Verify C matrix has correct dimensions"""
    spline = QuadraticBSpline([0, 1, 2, 3, 4])
    C = spline.build_derivative_constraint_matrix()

    # For k=2, should have (num_c1 + num_c2) constraint rows
    num_c0 = spline.num_coeff
    num_c1 = spline.num_coeffd
    num_c2 = spline.num_coeffdd

    assert C.shape[0] == num_c1 + num_c2
    assert C.shape[1] == num_c0 + num_c1 + num_c2

def test_derivative_relationship():
    """Verify C·c = 0 when coefficients satisfy derivative formula"""
    spline = QuadraticBSpline([0, 1, 2, 3, 4])
    spline.set_coeffs(np.array([1, 2, 3, 2, 1]))  # Arbitrary coeffs

    # Stack all coefficients: [c⁽⁰⁾, c⁽¹⁾, c⁽²⁾]
    c_all = np.hstack([spline.coeffs, spline.coeffsd, spline.coeffsdd])

    C = spline.build_derivative_constraint_matrix()
    residual = C @ c_all

    assert np.allclose(residual, 0, atol=1e-10)

def test_derivative_matrix_rank():
    """C should be full row rank (all constraints independent)"""
    spline = QuadraticBSpline([0, 1, 2, 3, 4, 5])
    C = spline.build_derivative_constraint_matrix()

    rank = np.linalg.matrix_rank(C.toarray())
    assert rank == C.shape[0]  # Full row rank
```

**Checkpoint 1.1**:
- ✅ C matrix builds without errors
- ✅ All three tests pass
- ✅ For a simple 5-knot spline, manually verify C structure is correct

---

### Step 1.2: Add Coefficient Evaluation Method

**Goal**: Evaluate spline directly from coefficient vector (needed for optimization).

**Implementation**:
```python
# In qbspline.py:

def evaluate_from_coefficients(self, c0, x):
    """
    Evaluate spline at x using provided coefficients c0.

    s(x) = Σ c0[i] * Bi,k+1(x)

    Args:
        c0: array of spline coefficients (length num_coeff)
        x: evaluation point or array

    Returns: spline value(s) at x
    """
    # Use active_basis(x) to get non-zero basis functions
    # Only sum over active basis - much faster
    pass

def evaluate_gradient_from_coefficients(self, c1, x):
    """
    Evaluate first derivative from derivative coefficients c1.

    s'(x) = 2 * Σ c1[i] * Bi,k(x)  (for k=2)
    """
    pass
```

**Test Plan**:
```python
# tests/test_spline_evaluation.py

def test_eval_from_coeffs_matches_existing():
    """New method should match existing eval() method"""
    spline = QuadraticBSpline([0, 1, 2, 3, 4])
    spline.set_coeffs(np.array([1, 2, 3, 2, 1]))

    test_points = np.linspace(0, 4, 20)

    for x in test_points:
        original = spline.eval(x)
        from_coeffs = spline.evaluate_from_coefficients(spline.coeffs, x)
        assert np.allclose(original, from_coeffs)

def test_gradient_eval():
    """Gradient evaluation should match derivative"""
    spline = QuadraticBSpline([0, 1, 2, 3, 4])
    spline.set_coeffs(np.array([1, 4, 9, 4, 1]))  # Parabolic

    test_points = np.linspace(0, 4, 20)

    for x in test_points:
        original = spline.evald(x)
        from_coeffs = spline.evaluate_gradient_from_coefficients(spline.coeffsd, x)
        assert np.allclose(original, from_coeffs)
```

**Checkpoint 1.2**:
- ✅ Evaluation from coefficients matches existing methods
- ✅ Can evaluate at multiple points efficiently
- ✅ Gradient evaluation works correctly

---

## Phase 2: Data Structures (Week 2)

### Step 2.1: Ground Profile Data Structure

**Goal**: Load and represent ground elevation data.

**Implementation**:
```python
# ground_profile.py

import numpy as np
from dataclasses import dataclass
from typing import List

@dataclass
class CrossSection:
    """Cross-sectional geometry at a station"""
    station: float              # Position along road (m)
    center_elevation: float     # Ground elevation at centerline (m)
    width: float                # Road width (m)
    left_slope_angle: float     # α in radians
    right_slope_angle: float    # β in radians
    section_length: float       # Distance to next station (m)

    def compute_true_volume(self, offset: float) -> float:
        """
        Compute exact volume for trapezoidal cross-section.

        V(u) = W·L·u + 0.5·L·u²·(1/tan(α) + 1/tan(β))

        Args:
            offset: vertical offset (negative = cut, positive = fill)

        Returns: volume in m³
        """
        if offset == 0:
            return 0.0

        A = self.width * self.section_length
        B = 0.5 * self.section_length * (
            1.0 / np.tan(self.left_slope_angle) +
            1.0 / np.tan(self.right_slope_angle)
        )

        return A * offset + B * offset**2

class GroundProfile:
    """Ground elevation profile along road centerline"""

    def __init__(self, csv_path: str):
        """
        Load from CSV with columns: station, elevation, width, left_slope, right_slope
        """
        data = np.loadtxt(csv_path, delimiter=',', skiprows=1)

        self.stations = data[:, 0]
        self.elevations = data[:, 1]

        # Build cross-sections
        self.cross_sections = []
        for i in range(len(data) - 1):
            section_length = self.stations[i+1] - self.stations[i]

            cs = CrossSection(
                station=data[i, 0],
                center_elevation=data[i, 1],
                width=data[i, 2],
                left_slope_angle=np.radians(data[i, 3]),  # Convert to radians
                right_slope_angle=np.radians(data[i, 4]),
                section_length=section_length
            )
            self.cross_sections.append(cs)

    def elevation_at(self, station: float) -> float:
        """Interpolate ground elevation at any station"""
        return np.interp(station, self.stations, self.elevations)
```

**Test Plan**:
```python
# tests/test_ground_profile.py

def test_load_ground_profile():
    """Load sample ground profile"""
    # Create test CSV
    test_data = """station,elevation,width,left_slope,right_slope
0,100,10,45,45
10,102,10,45,45
20,105,10,45,45
30,103,10,45,45"""

    with open('test_ground.csv', 'w') as f:
        f.write(test_data)

    ground = GroundProfile('test_ground.csv')

    assert len(ground.stations) == 4
    assert len(ground.cross_sections) == 3
    assert ground.elevations[0] == 100

    os.remove('test_ground.csv')

def test_volume_calculation():
    """Verify trapezoidal volume formula"""
    cs = CrossSection(
        station=0,
        center_elevation=100,
        width=10,
        left_slope_angle=np.radians(45),
        right_slope_angle=np.radians(45),
        section_length=10
    )

    # For 45° slopes, tan(45°) = 1
    # V = 10*10*u + 0.5*10*u²*(1 + 1) = 100u + 10u²

    offset = 2.0  # 2m fill
    expected = 100 * 2 + 10 * 4  # = 240 m³
    actual = cs.compute_true_volume(offset)

    assert np.isclose(actual, expected)

def test_elevation_interpolation():
    """Test ground elevation interpolation"""
    # Simple test data
    test_data = """station,elevation,width,left_slope,right_slope
0,100,10,45,45
10,110,10,45,45"""

    with open('test_interp.csv', 'w') as f:
        f.write(test_data)

    ground = GroundProfile('test_interp.csv')

    # Should interpolate linearly
    assert ground.elevation_at(5) == 105
    assert ground.elevation_at(7) == 107

    os.remove('test_interp.csv')
```

**Checkpoint 2.1**:
- ✅ Can load CSV ground profile
- ✅ Volume calculation matches analytical formula
- ✅ Elevation interpolation works

---

### Step 2.2: Configuration and Cost Parameters

**Goal**: Define road design constraints and costs.

**Implementation**:
```python
# config.py

from dataclasses import dataclass

@dataclass
class RoadConfig:
    """Road design constraints"""
    min_grade: float = -0.08        # -8% maximum downgrade
    max_grade: float = 0.08         # +8% maximum upgrade
    max_curvature: float = 0.01     # Maximum curvature (1/m)
    spline_degree: int = 2          # Quadratic splines

@dataclass
class CostParams:
    """Unit costs for earthwork operations"""
    excavation_cost: float = 5.0    # $/m³ excavation
    embankment_cost: float = 4.0    # $/m³ embankment
    loading_cost: float = 1.0       # $/m³ loading
    haul_cost_per_meter: float = 0.1  # $/m³/m hauling
```

**Test**: Just verify dataclasses work
```python
def test_config_creation():
    config = RoadConfig()
    assert config.min_grade == -0.08

    costs = CostParams(excavation_cost=10.0)
    assert costs.excavation_cost == 10.0
```

**Checkpoint 2.2**:
- ✅ Configuration objects created successfully

---

## Phase 3: Volume Approximation (Week 3)

### Step 3.1: Least-Squares Quadratic Fitting

**Goal**: For each cross-section, compute χ₁, χ₂, χ₃ coefficients.

**Implementation**:
```python
# volume_approximation.py

import numpy as np
from typing import Tuple
from ground_profile import CrossSection

def fit_volume_coefficients(cross_section: CrossSection,
                           offset_range: np.ndarray,
                           is_cut: bool) -> Tuple[np.ndarray, float]:
    """
    Fit quadratic approximation to volume: V ≥ χ₁u² + χ₂u + χ₃

    Args:
        cross_section: Cross-section geometry
        offset_range: Sample offsets for fitting (e.g., -5 to 0 for cut)
        is_cut: True for cut sections, False for fill

    Returns:
        chi: (χ₁, χ₂, χ₃) coefficients
        r_squared: Goodness of fit
    """
    # Generate true volumes at sample offsets
    volumes = np.array([
        cross_section.compute_true_volume(u) for u in offset_range
    ])

    # Fit quadratic: V = χ₁u² + χ₂u + χ₃
    # Using np.polyfit (returns coefficients in descending order)
    coeffs = np.polyfit(offset_range, volumes, deg=2)
    chi = np.array([coeffs[0], coeffs[1], coeffs[2]])  # [χ₁, χ₂, χ₃]

    # Compute R²
    fitted_volumes = chi[0] * offset_range**2 + chi[1] * offset_range + chi[2]
    residuals = volumes - fitted_volumes
    ss_res = np.sum(residuals**2)
    ss_tot = np.sum((volumes - np.mean(volumes))**2)
    r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 1.0

    return chi, r_squared

def compute_all_volume_coefficients(cross_sections: list,
                                   cut_offset_range: np.ndarray = None,
                                   fill_offset_range: np.ndarray = None,
                                   r_squared_threshold: float = 0.95):
    """
    Compute volume coefficients for all cross-sections.

    Args:
        cross_sections: List of CrossSection objects
        cut_offset_range: Sample offsets for cut (default: -10 to 0)
        fill_offset_range: Sample offsets for fill (default: 0 to 10)
        r_squared_threshold: If R² below this, use linear approximation

    Returns:
        coeffs_cut: List of (χ₁, χ₂, χ₃) for cut at each section
        coeffs_fill: List of (χ₁, χ₂, χ₃) for fill at each section
        approx_types: List of 'quadratic' or 'linear' for each section
    """
    if cut_offset_range is None:
        cut_offset_range = np.linspace(-10, 0, 50)
    if fill_offset_range is None:
        fill_offset_range = np.linspace(0, 10, 50)

    coeffs_cut = []
    coeffs_fill = []
    approx_types = []

    for cs in cross_sections:
        # Fit cut
        chi_cut, r2_cut = fit_volume_coefficients(cs, cut_offset_range, is_cut=True)

        # Fit fill
        chi_fill, r2_fill = fit_volume_coefficients(cs, fill_offset_range, is_cut=False)

        # Check convexity and R²
        if chi_cut[0] < 0 or r2_cut < r_squared_threshold:
            # Fall back to linear: set χ₁ = 0
            chi_cut[0] = 0
            approx_type = 'linear'
        else:
            approx_type = 'quadratic'

        if chi_fill[0] < 0 or r2_fill < r_squared_threshold:
            chi_fill[0] = 0
            approx_type = 'linear'

        coeffs_cut.append(chi_cut)
        coeffs_fill.append(chi_fill)
        approx_types.append(approx_type)

    return coeffs_cut, coeffs_fill, approx_types
```

**Test Plan**:
```python
# tests/test_volume_approximation.py

def test_fit_symmetric_section():
    """For symmetric 45° slopes, verify analytical formula"""
    cs = CrossSection(
        station=0,
        center_elevation=100,
        width=10,
        left_slope_angle=np.radians(45),
        right_slope_angle=np.radians(45),
        section_length=10
    )

    # True formula: V = 100u + 10u²
    # χ₁ should be ≈ 10, χ₂ ≈ 100

    offset_range = np.linspace(-5, 0, 20)
    chi, r2 = fit_volume_coefficients(cs, offset_range, is_cut=True)

    assert np.isclose(chi[0], 10, rtol=0.01)  # χ₁
    assert np.isclose(chi[1], 100, rtol=0.01)  # χ₂
    assert r2 > 0.999  # Should be nearly perfect fit

def test_positive_chi1_for_convexity():
    """χ₁ must be positive for convexity"""
    cs = CrossSection(
        station=0,
        center_elevation=100,
        width=10,
        left_slope_angle=np.radians(45),
        right_slope_angle=np.radians(45),
        section_length=10
    )

    offset_range = np.linspace(0, 5, 20)
    chi, r2 = fit_volume_coefficients(cs, offset_range, is_cut=False)

    assert chi[0] > 0, "Not convex!"

def test_linear_fallback():
    """If R² low, should fall back to linear"""
    # Create pathological cross-section (won't happen in practice)
    cs = CrossSection(
        station=0,
        center_elevation=100,
        width=10,
        left_slope_angle=np.radians(89),  # Nearly vertical
        right_slope_angle=np.radians(89),
        section_length=10
    )

    coeffs_cut, coeffs_fill, types = compute_all_volume_coefficients(
        [cs], r_squared_threshold=0.99
    )

    # Might fall back to linear
    if types[0] == 'linear':
        assert coeffs_cut[0][0] == 0  # χ₁ set to zero
```

**Checkpoint 3.1**:
- ✅ Quadratic fit achieves high R² (> 0.95) for typical slopes
- ✅ χ₁ is positive (convex)
- ✅ Linear fallback works when R² is low

---

## Phase 4: QP Formulation (Week 4-5)

### Step 4.1: Decision Vector Assembly

**Goal**: Define the structure of the optimization variable vector.

**Implementation**:
```python
# qp_formulation.py

import numpy as np
from typing import Dict, Tuple
from qbspline import QuadraticBSpline
from ground_profile import GroundProfile

class VariableIndexer:
    """Manages indices of different variable types in decision vector"""

    def __init__(self, spline: QuadraticBSpline, num_sections: int):
        """
        Decision vector structure:
        x = [c⁽⁰⁾, c⁽¹⁾, c⁽²⁾, u, V⁺, V⁻]
        """
        self.num_c0 = spline.num_coeff
        self.num_c1 = spline.num_coeffd
        self.num_c2 = spline.num_coeffdd
        self.num_sections = num_sections

        # Compute start indices
        self.c0_start = 0
        self.c1_start = self.c0_start + self.num_c0
        self.c2_start = self.c1_start + self.num_c1
        self.u_start = self.c2_start + self.num_c2
        self.vplus_start = self.u_start + num_sections
        self.vminus_start = self.vplus_start + num_sections

        self.total_vars = self.vminus_start + num_sections

    def c0_slice(self) -> slice:
        return slice(self.c0_start, self.c1_start)

    def c1_slice(self) -> slice:
        return slice(self.c1_start, self.c2_start)

    def c2_slice(self) -> slice:
        return slice(self.c2_start, self.u_start)

    def u_slice(self) -> slice:
        return slice(self.u_start, self.vplus_start)

    def vplus_slice(self) -> slice:
        return slice(self.vplus_start, self.vminus_start)

    def vminus_slice(self) -> slice:
        return slice(self.vminus_start, self.total_vars)

    def get_var_index(self, var_type: str, index: int) -> int:
        """Get absolute index of a specific variable"""
        if var_type == 'c0':
            return self.c0_start + index
        elif var_type == 'c1':
            return self.c1_start + index
        elif var_type == 'c2':
            return self.c2_start + index
        elif var_type == 'u':
            return self.u_start + index
        elif var_type == 'vplus':
            return self.vplus_start + index
        elif var_type == 'vminus':
            return self.vminus_start + index
        else:
            raise ValueError(f"Unknown variable type: {var_type}")
```

**Test**:
```python
def test_variable_indexer():
    """Verify variable indexing is correct"""
    spline = QuadraticBSpline([0, 1, 2, 3, 4])
    num_sections = 3

    indexer = VariableIndexer(spline, num_sections)

    # Check sizes
    assert indexer.num_c0 == spline.num_coeff
    assert indexer.num_c1 == spline.num_coeffd
    assert indexer.num_c2 == spline.num_coeffdd

    # Check non-overlapping
    assert indexer.c1_start == indexer.c0_start + indexer.num_c0
    assert indexer.u_start > indexer.c2_start

    # Check total
    expected_total = (indexer.num_c0 + indexer.num_c1 + indexer.num_c2 +
                     3 * num_sections)
    assert indexer.total_vars == expected_total
```

**Checkpoint 4.1**:
- ✅ Variable indexer correctly maps variable types to indices
- ✅ Total variable count is correct

---

### Step 4.2: Build Derivative Constraints (C·c = 0)

**Implementation**:
```python
# In qp_formulation.py:

def build_derivative_constraints(spline: QuadraticBSpline,
                                indexer: VariableIndexer) -> Tuple[np.ndarray, np.ndarray]:
    """
    Build A_eq·x = b_eq for derivative relationships.

    Returns:
        A_eq: Constraint matrix (only affects c⁽⁰⁾, c⁽¹⁾, c⁽²⁾ variables)
        b_eq: Right-hand side (all zeros)
    """
    # Get C matrix from spline (only for coefficient variables)
    C = spline.build_derivative_constraint_matrix()

    # Expand to full decision vector (pad with zeros for u, V⁺, V⁻)
    num_rows = C.shape[0]
    A_eq = np.zeros((num_rows, indexer.total_vars))

    # Fill in C matrix for coefficient variables
    coeff_end = indexer.u_start
    A_eq[:, :coeff_end] = C.toarray() if hasattr(C, 'toarray') else C

    b_eq = np.zeros(num_rows)

    return A_eq, b_eq
```

**Test**:
```python
def test_derivative_constraints_shape():
    spline = QuadraticBSpline([0, 1, 2, 3, 4])
    indexer = VariableIndexer(spline, num_sections=3)

    A_eq, b_eq = build_derivative_constraints(spline, indexer)

    assert A_eq.shape[1] == indexer.total_vars
    assert len(b_eq) == A_eq.shape[0]
    assert np.all(b_eq == 0)
```

**Checkpoint 4.2**:
- ✅ Derivative constraint matrix builds correctly
- ✅ Dimensions match decision vector size

---

### Step 4.3: Build Gap Constraints

**Implementation**:
```python
def build_gap_constraints(spline: QuadraticBSpline,
                         ground: GroundProfile,
                         indexer: VariableIndexer) -> Tuple[np.ndarray, np.ndarray]:
    """
    Build constraints: P(xⱼ) - Zⱼ = uⱼ

    where P(xⱼ) = Σ c⁽⁰⁾ᵢ Bᵢ,ₖ₊₁(xⱼ)

    Rearranged: Σ c⁽⁰⁾ᵢ Bᵢ,ₖ₊₁(xⱼ) - uⱼ = Zⱼ
    """
    num_sections = indexer.num_sections
    A_eq = np.zeros((num_sections, indexer.total_vars))
    b_eq = np.zeros(num_sections)

    for j in range(num_sections):
        station = ground.cross_sections[j].station

        # Get active basis functions at this station
        active_indices = spline.active_basis(station)

        # Evaluate basis functions
        for idx in active_indices:
            basis_value = spline.N2(idx, station)
            A_eq[j, indexer.get_var_index('c0', idx)] = basis_value

        # Offset term: -uⱼ
        A_eq[j, indexer.get_var_index('u', j)] = -1.0

        # RHS: Zⱼ
        b_eq[j] = ground.cross_sections[j].center_elevation

    return A_eq, b_eq
```

**Test**:
```python
def test_gap_constraints():
    """Test gap constraints P(xⱼ) - Zⱼ = uⱼ"""
    spline = QuadraticBSpline([0, 1, 2, 3])

    # Create simple ground profile
    import tempfile
    with tempfile.NamedTemporaryFile(mode='w', suffix='.csv', delete=False) as f:
        f.write("station,elevation,width,left_slope,right_slope\n")
        f.write("0,100,10,45,45\n")
        f.write("1,101,10,45,45\n")
        f.write("2,102,10,45,45\n")
        csv_path = f.name

    ground = GroundProfile(csv_path)
    indexer = VariableIndexer(spline, num_sections=2)

    A_eq, b_eq = build_gap_constraints(spline, ground, indexer)

    assert A_eq.shape == (2, indexer.total_vars)
    assert b_eq[0] == 100
    assert b_eq[1] == 101

    os.remove(csv_path)
```

**Checkpoint 4.3**:
- ✅ Gap constraints build correctly
- ✅ RHS contains ground elevations
- ✅ Constraint matrix has correct structure

---

### Step 4.4: Build Objective Function

**Implementation**:
```python
def build_objective(indexer: VariableIndexer,
                   costs: CostParams) -> Tuple[np.ndarray, np.ndarray]:
    """
    Build objective: minimize (p + y)·ΣV⁺ + q·ΣV⁻ + haul_costs

    For MVP: haul costs are linearized (material moves to adjacent sections).

    Returns:
        Q: Quadratic term matrix (all zeros for linear objective)
        c: Linear cost vector
    """
    Q = np.zeros((indexer.total_vars, indexer.total_vars))
    c = np.zeros(indexer.total_vars)

    # Cut volume costs: (p + y) per m³
    cut_cost = costs.excavation_cost + costs.loading_cost
    c[indexer.vplus_slice()] = cut_cost

    # Fill volume costs: q per m³
    fill_cost = costs.embankment_cost
    c[indexer.vminus_slice()] = fill_cost

    # Haul costs: for MVP, assume fixed distance between sections
    # This is simplified - full version would have flow variables
    # For now, just include average haul cost in volume costs
    avg_haul = costs.haul_cost_per_meter * 50  # Assume 50m average
    c[indexer.vplus_slice()] += avg_haul
    c[indexer.vminus_slice()] += avg_haul

    return Q, c
```

**Test**:
```python
def test_objective_structure():
    """Verify objective function structure"""
    spline = QuadraticBSpline([0, 1, 2, 3, 4])
    indexer = VariableIndexer(spline, num_sections=3)
    costs = CostParams()

    Q, c = build_objective(indexer, costs)

    assert Q.shape == (indexer.total_vars, indexer.total_vars)
    assert len(c) == indexer.total_vars

    # Q should be all zeros (linear objective)
    assert np.all(Q == 0)

    # Only volume variables should have non-zero costs
    assert np.all(c[:indexer.u_start] == 0)  # Coefficients have zero cost
    assert np.all(c[indexer.u_slice()] == 0)  # Offsets have zero cost
    assert np.all(c[indexer.vplus_slice()] > 0)  # Cut costs
    assert np.all(c[indexer.vminus_slice()] > 0)  # Fill costs
```

**Checkpoint 4.4**:
- ✅ Objective builds correctly
- ✅ Only volume variables have costs
- ✅ Q matrix is zero (linear objective for MVP)

---

### Step 4.5: Build Bounds

**Implementation**:
```python
def build_bounds(indexer: VariableIndexer,
                config: RoadConfig,
                offset_limits: Tuple[float, float] = (-20, 20)) -> Tuple[np.ndarray, np.ndarray]:
    """
    Build variable bounds.

    - c⁽⁰⁾: unbounded
    - c⁽¹⁾: G_min ≤ c⁽¹⁾ ≤ G_max (DIRECT grade constraints!)
    - c⁽²⁾: -K_max ≤ c⁽²⁾ ≤ K_max (curvature limits)
    - u: offset_limits
    - V⁺, V⁻: ≥ 0
    """
    lb = np.full(indexer.total_vars, -np.inf)
    ub = np.full(indexer.total_vars, np.inf)

    # Gradient bounds (DIRECT on c⁽¹⁾ coefficients)
    lb[indexer.c1_slice()] = config.min_grade
    ub[indexer.c1_slice()] = config.max_grade

    # Curvature bounds
    lb[indexer.c2_slice()] = -config.max_curvature
    ub[indexer.c2_slice()] = config.max_curvature

    # Offset bounds
    lb[indexer.u_slice()] = offset_limits[0]
    ub[indexer.u_slice()] = offset_limits[1]

    # Volume bounds (non-negative)
    lb[indexer.vplus_slice()] = 0
    lb[indexer.vminus_slice()] = 0

    return lb, ub
```

**Test**:
```python
def test_bounds():
    """Verify bounds are set correctly"""
    spline = QuadraticBSpline([0, 1, 2, 3, 4])
    indexer = VariableIndexer(spline, num_sections=3)
    config = RoadConfig(min_grade=-0.08, max_grade=0.08)

    lb, ub = build_bounds(indexer, config)

    # Check gradient bounds
    assert np.all(lb[indexer.c1_slice()] == -0.08)
    assert np.all(ub[indexer.c1_slice()] == 0.08)

    # Check volume bounds
    assert np.all(lb[indexer.vplus_slice()] == 0)
    assert np.all(lb[indexer.vminus_slice()] == 0)
```

**Checkpoint 4.5**:
- ✅ Bounds set correctly for all variable types
- ✅ Grade limits applied directly to c⁽¹⁾ coefficients

---

## Phase 5: ALGLIB Integration (Week 6)

### Step 5.1: ALGLIB Solver Wrapper

**Implementation**:
```python
# alglib_solver.py

import alglib
import numpy as np
from typing import Tuple, Dict

def solve_qp(Q: np.ndarray,
            c: np.ndarray,
            A_eq: np.ndarray,
            b_eq: np.ndarray,
            lb: np.ndarray,
            ub: np.ndarray) -> Dict:
    """
    Solve QP using ALGLIB minqp solver.

    min  0.5·x^T·Q·x + c^T·x
    s.t. A_eq·x = b_eq
         lb ≤ x ≤ ub

    Returns:
        solution: dict with 'x', 'optimal_value', 'status', 'iterations'
    """
    n = len(c)

    # Create QP state
    state = alglib.minqpcreate(n)

    # Set quadratic term
    alglib.minqpsetquadraticterm(state, Q.tolist())

    # Set linear term
    alglib.minqpsetlinearterm(state, c.tolist())

    # Set linear equality constraints
    if A_eq.shape[0] > 0:
        # ALGLIB format: each row is [a1, a2, ..., an, b]
        # For constraint: a1*x1 + a2*x2 + ... + an*xn = b
        constraint_matrix = np.column_stack([A_eq, b_eq])
        constraint_types = np.zeros(A_eq.shape[0])  # 0 = equality
        alglib.minqpsetlc(state, constraint_matrix.tolist(), constraint_types.tolist())

    # Set bounds
    alglib.minqpsetbc(state, lb.tolist(), ub.tolist())

    # Solve with QuickQP algorithm
    alglib.minqpsetalgoquickqp(state, 0.0, 0.0, 0.0, 0, True)
    alglib.minqpoptimize(state)

    # Extract results
    x, rep = alglib.minqpresults(state)

    return {
        'x': np.array(x),
        'status': rep.terminationtype,  # >0 = success
        'iterations': rep.iterationscount,
        'optimal_value': 0.5 * x @ Q @ x + c @ x
    }
```

**Test Plan**:
```python
# tests/test_alglib_solver.py

def test_simple_qp():
    """Test with known simple QP"""
    # min x² + y² subject to x + y = 1
    Q = np.array([[2, 0], [0, 2]], dtype=float)
    c = np.array([0, 0], dtype=float)
    A_eq = np.array([[1, 1]], dtype=float)
    b_eq = np.array([1], dtype=float)
    lb = np.array([-np.inf, -np.inf])
    ub = np.array([np.inf, np.inf])

    result = solve_qp(Q, c, A_eq, b_eq, lb, ub)

    assert result['status'] > 0  # Success
    assert np.allclose(result['x'], [0.5, 0.5], atol=1e-6)

def test_bounded_qp():
    """Test with bounds"""
    # min x² subject to 0 ≤ x ≤ 1
    Q = np.array([[2]], dtype=float)
    c = np.array([0], dtype=float)
    A_eq = np.zeros((0, 1))
    b_eq = np.zeros(0)
    lb = np.array([0])
    ub = np.array([1])

    result = solve_qp(Q, c, A_eq, b_eq, lb, ub)

    assert result['status'] > 0
    assert np.isclose(result['x'][0], 0, atol=1e-6)  # Optimal at lower bound
```

**Checkpoint 5.1**:
- ✅ ALGLIB solver works on simple test problems
- ✅ Equality constraints are enforced
- ✅ Bounds are respected

---

## Phase 6: Integration and Testing (Week 7)

### Step 6.1: End-to-End Pipeline

**Implementation**:
```python
# main.py

import numpy as np
from qbspline import QuadraticBSpline
from ground_profile import GroundProfile
from volume_approximation import compute_all_volume_coefficients
from qp_formulation import *
from config import RoadConfig, CostParams
from alglib_solver import solve_qp

def optimize_road_alignment(ground_csv: str,
                           config: RoadConfig = None,
                           costs: CostParams = None):
    """
    Main optimization pipeline.

    1. Load ground profile
    2. Create spline with candidate knots
    3. Compute volume coefficients
    4. Build QP
    5. Solve
    6. Extract solution
    """
    if config is None:
        config = RoadConfig()
    if costs is None:
        costs = CostParams()

    # Load ground
    print("Loading ground profile...")
    ground = GroundProfile(ground_csv)
    print(f"  {len(ground.stations)} stations, {len(ground.cross_sections)} sections")

    # Create spline with candidate knots
    print("Creating spline...")
    # Use ground stations as candidate knots
    candidate_knots = ground.stations
    spline = QuadraticBSpline(candidate_knots)
    print(f"  {len(candidate_knots)} candidate knots")
    print(f"  {spline.num_coeff} spline coefficients")

    # Compute volume coefficients
    print("Computing volume approximations...")
    coeffs_cut, coeffs_fill, types = compute_all_volume_coefficients(
        ground.cross_sections
    )
    print(f"  {sum(1 for t in types if t == 'quadratic')} quadratic, "
          f"{sum(1 for t in types if t == 'linear')} linear")

    # Build QP
    print("Building QP formulation...")
    indexer = VariableIndexer(spline, len(ground.cross_sections))
    print(f"  Total variables: {indexer.total_vars}")

    # Objective
    Q, c = build_objective(indexer, costs)

    # Constraints
    A_deriv, b_deriv = build_derivative_constraints(spline, indexer)
    A_gap, b_gap = build_gap_constraints(spline, ground, indexer)

    # Combine equality constraints
    A_eq = np.vstack([A_deriv, A_gap])
    b_eq = np.hstack([b_deriv, b_gap])
    print(f"  Equality constraints: {A_eq.shape[0]}")

    # Bounds
    lb, ub = build_bounds(indexer, config)

    # Solve
    print("Solving QP...")
    result = solve_qp(Q, c, A_eq, b_eq, lb, ub)

    if result['status'] <= 0:
        print(f"  ERROR: Solver failed with status {result['status']}")
        return None

    print(f"  Success! Iterations: {result['iterations']}")
    print(f"  Optimal cost: ${result['optimal_value']:.2f}")

    # Extract solution
    x = result['x']
    solution = {
        'c0': x[indexer.c0_slice()],
        'c1': x[indexer.c1_slice()],
        'c2': x[indexer.c2_slice()],
        'offsets': x[indexer.u_slice()],
        'volumes_cut': x[indexer.vplus_slice()],
        'volumes_fill': x[indexer.vminus_slice()],
        'total_cost': result['optimal_value'],
        'spline': spline,
        'ground': ground
    }

    return solution

if __name__ == '__main__':
    import sys

    if len(sys.argv) < 2:
        print("Usage: python main.py <ground_profile.csv>")
        sys.exit(1)

    solution = optimize_road_alignment(sys.argv[1])

    if solution:
        print("\nSolution summary:")
        print(f"  Total cut: {np.sum(solution['volumes_cut']):.2f} m³")
        print(f"  Total fill: {np.sum(solution['volumes_fill']):.2f} m³")
        print(f"  Max grade: {np.max(solution['c1']):.3f}")
        print(f"  Min grade: {np.min(solution['c1']):.3f}")
```

**Test with Simple Synthetic Data**:
```python
# Create test data: flat ground
test_data = """station,elevation,width,left_slope,right_slope
0,100,10,45,45
100,100,10,45,45
200,100,10,45,45
300,100,10,45,45
400,100,10,45,45"""

with open('flat_ground.csv', 'w') as f:
    f.write(test_data)

# Run
solution = optimize_road_alignment('flat_ground.csv')

# Expected: Road should follow ground exactly (zero cost)
assert np.allclose(solution['offsets'], 0, atol=1e-6)
assert np.allclose(solution['volumes_cut'], 0, atol=1e-6)
assert np.allclose(solution['volumes_fill'], 0, atol=1e-6)
```

**Checkpoint 6.1**:
- ✅ Pipeline runs without errors
- ✅ For flat ground, road follows ground exactly
- ✅ Constraint violations are zero

---

### Step 6.2: Visualization

**Implementation**:
```python
# visualization.py

import matplotlib.pyplot as plt
import numpy as np

def plot_solution(solution: dict, save_path: str = None):
    """
    Create 4-panel visualization of solution.
    """
    fig, axes = plt.subplots(4, 1, figsize=(14, 12))

    # Extract data
    ground = solution['ground']
    spline = solution['spline']
    c0 = solution['c0']
    offsets = solution['offsets']

    stations = ground.stations[:-1]  # Cross-sections (n-1)

    # Evaluate road profile
    road_stations = np.linspace(stations[0], stations[-1], 200)
    road_elevations = np.array([
        spline.evaluate_from_coefficients(c0, s) for s in road_stations
    ])

    # Panel 1: Elevation profile
    axes[0].plot(ground.stations, ground.elevations, 'o-',
                color='brown', label='Ground', linewidth=2)
    axes[0].plot(road_stations, road_elevations, '-',
                color='black', label='Road', linewidth=2)

    # Shade cut/fill regions
    for i, offset in enumerate(offsets):
        x_left = stations[i]
        x_right = stations[i+1] if i < len(stations)-1 else stations[i]

        if offset < 0:  # Cut
            axes[0].axvspan(x_left, x_right, alpha=0.2, color='red')
        elif offset > 0:  # Fill
            axes[0].axvspan(x_left, x_right, alpha=0.2, color='blue')

    axes[0].set_ylabel('Elevation (m)', fontsize=12)
    axes[0].legend()
    axes[0].grid(True, alpha=0.3)
    axes[0].set_title('Road Alignment Profile', fontsize=14, fontweight='bold')

    # Panel 2: Gradient
    c1 = solution['c1']
    gradient_stations = np.linspace(stations[0], stations[-1], len(c1))
    axes[1].plot(gradient_stations, c1, 'g-', linewidth=2)
    axes[1].axhline(0.08, color='r', linestyle='--', label='Grade limits')
    axes[1].axhline(-0.08, color='r', linestyle='--')
    axes[1].set_ylabel('Gradient', fontsize=12)
    axes[1].legend()
    axes[1].grid(True, alpha=0.3)
    axes[1].set_title('Road Gradient', fontsize=14, fontweight='bold')

    # Panel 3: Curvature
    c2 = solution['c2']
    curv_stations = np.linspace(stations[0], stations[-1], len(c2))
    axes[2].plot(curv_stations, c2, 'm-', linewidth=2)
    axes[2].set_ylabel('Curvature (1/m)', fontsize=12)
    axes[2].grid(True, alpha=0.3)
    axes[2].set_title('Road Curvature', fontsize=14, fontweight='bold')

    # Panel 4: Volumes
    width = stations[1] - stations[0] if len(stations) > 1 else 10
    axes[3].bar(stations, solution['volumes_cut'], width=width*0.8,
               color='red', alpha=0.6, label='Cut')
    axes[3].bar(stations, -solution['volumes_fill'], width=width*0.8,
               color='blue', alpha=0.6, label='Fill')
    axes[3].set_xlabel('Station (m)', fontsize=12)
    axes[3].set_ylabel('Volume (m³)', fontsize=12)
    axes[3].legend()
    axes[3].grid(True, alpha=0.3)
    axes[3].set_title('Earthwork Volumes', fontsize=14, fontweight='bold')

    plt.tight_layout()

    if save_path:
        plt.savefig(save_path, dpi=150, bbox_inches='tight')

    plt.show()

    return fig
```

**Checkpoint 6.2**:
- ✅ Visualization shows all key aspects
- ✅ Cut/fill regions are clear
- ✅ Grade limits are visible

---

## Phase 7: Validation (Week 8)

### Step 7.1: Synthetic Test Cases

Create test cases with known solutions:

```python
# tests/test_integration.py

def test_flat_ground():
    """Flat ground → road should match ground (zero cost)"""
    # Already tested in 6.1

def test_linear_slope_within_limits():
    """Linear slope < max grade → road should follow ground"""
    # Create 5% uphill slope
    test_data = """station,elevation,width,left_slope,right_slope
0,100,10,45,45
100,105,10,45,45
200,110,10,45,45"""

    # Run optimization
    # Expect: road follows ground (5% < 8% limit)

def test_steep_slope_exceeds_limits():
    """Steep slope > max grade → road should use maximum allowed grade"""
    # Create 15% slope (exceeds 8% limit)
    test_data = """station,elevation,width,left_slope,right_slope
0,100,10,45,45
100,115,10,45,45
200,130,10,45,45"""

    # Run optimization
    # Expect: road uses 8% grade, creates cut at start, fill at end

def test_balancing_cut_fill():
    """Verify cut and fill are balanced"""
    # Any solution should have similar cut and fill volumes
    # (within some tolerance for haul distance limits)
```

**Checkpoint 7.1**:
- ✅ All synthetic tests pass
- ✅ Solution makes physical sense

---

### Step 7.2: Compare with Paper Benchmarks

If you have access to data from the 2025 paper:

```python
def test_paper_road_1():
    """Compare with results from 2025 paper Table 5"""
    # Load road data from paper
    # Run optimization
    # Compare:
    #   - MAPE (mean absolute percentage error for volumes)
    #   - Total cost
    #   - Number of active knots
```

**Checkpoint 7.2**:
- ✅ Results are comparable to paper (within 10%)
- ✅ Volume approximation MAPE < 5%

---

## Success Criteria Checklist

Before considering MVP complete:

- [ ] **Code Quality**:
  - [ ] All tests pass
  - [ ] No warnings or errors
  - [ ] Code is documented

- [ ] **Functionality**:
  - [ ] Can load ground profile from CSV
  - [ ] Solves optimization successfully
  - [ ] Produces valid road alignment
  - [ ] Visualization works

- [ ] **Correctness**:
  - [ ] Flat ground test passes
  - [ ] Grade limits are respected (|c⁽¹⁾| ≤ G_max)
  - [ ] Volumes are non-negative
  - [ ] Derivative relationships hold (C·c ≈ 0)
  - [ ] Gap constraints satisfied (road - ground = offset)

- [ ] **Performance**:
  - [ ] Solves 100-section problem in < 10 seconds
  - [ ] Memory usage is reasonable (< 1GB)

- [ ] **Documentation**:
  - [ ] CLAUDE.md is up to date
  - [ ] README explains how to run
  - [ ] Example data files included

---

## Common Pitfalls and Solutions

### Pitfall 1: Infeasible QP
**Symptom**: ALGLIB reports negative status, no solution found

**Debug steps**:
1. Check if ground slope exceeds grade limits
2. Verify derivative constraint matrix C is correct
3. Simplify: try with only 5 sections first
4. Check for contradictory constraints

### Pitfall 2: Poor Volume Approximation
**Symptom**: High MAPE, volumes don't match true volumes

**Debug steps**:
1. Plot true vs fitted volumes for one section
2. Check if χ₁ > 0 (convexity)
3. Increase offset sample density
4. Check if R² < 0.95 (should use linear fallback)

### Pitfall 3: Grade Constraints Violated
**Symptom**: Solution has gradients outside [G_min, G_max]

**Debug steps**:
1. Print c⁽¹⁾ values - should be within bounds
2. Check if bounds were set correctly in build_bounds()
3. Verify indexer maps c⁽¹⁾ variables correctly

### Pitfall 4: Solution Doesn't Look Smooth
**Symptom**: Road has discontinuities or kinks

**Debug steps**:
1. Verify C·c ≈ 0 (derivative relationships)
2. Check knot vector has proper boundary padding
3. Evaluate spline at many points to see detail

---

## Next Steps After MVP

Once MVP is working:

1. **Add flow constraints**: Proper material balance (not just volume minimization)
2. **Multiple haul paths**: Equipment selection optimization
3. **Multiple materials**: Separate volume variables per material type
4. **Regularization**: L1-norm to reduce number of active knots
5. **Validation**: Test on real road data from paper

---

## Time Estimates

- **Week 1**: B-spline foundation (Steps 1.1-1.2)
- **Week 2**: Data structures (Steps 2.1-2.2)
- **Week 3**: Volume approximation (Step 3.1)
- **Week 4-5**: QP formulation (Steps 4.1-4.5)
- **Week 6**: ALGLIB integration (Step 5.1)
- **Week 7**: Integration and visualization (Steps 6.1-6.2)
- **Week 8**: Validation and debugging (Steps 7.1-7.2)

**Total: ~8 weeks for a working MVP**

Adjust based on your experience level and time availability. The key is to test at each checkpoint before proceeding!
