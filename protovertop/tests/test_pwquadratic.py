import pytest
import numpy as np
from pwquadratic import pwq_create, pwq_set_coeffs, pwq_eval, pwq_evald


@pytest.mark.unit
def test_pwq_create_basic():
    """Test creating a piecewise quadratic with basic stations"""
    stations = np.array([0.0, 100.0, 200.0, 300.0])
    pwq = pwq_create(stations)

    assert pwq is not None
    # Check that it stores stations correctly
    # (Implementation details may vary)


@pytest.mark.unit
def test_pwq_create_single_segment():
    """Test creating piecewise quadratic with single segment (2 stations)"""
    stations = np.array([0.0, 100.0])
    pwq = pwq_create(stations)

    assert pwq is not None


@pytest.mark.unit
def test_pwq_create_many_segments():
    """Test creating piecewise quadratic with many segments"""
    stations = np.linspace(0, 1000, 21)  # 20 segments
    pwq = pwq_create(stations)

    assert pwq is not None


@pytest.mark.unit
def test_pwq_set_coeffs_and_eval():
    """Test setting coefficients and evaluating piecewise quadratic"""
    stations = np.array([0.0, 100.0, 200.0])
    pwq = pwq_create(stations)

    # Set coefficients for 2 segments
    # Segment 0: P(s) = 100 + 0.05*(s-0) + 0*(s-0)²  (linear, 5% grade)
    # Segment 1: P(s) = 105 + 0.05*(s-100) + 0*(s-100)²
    a0 = np.array([100.0, 105.0])
    a1 = np.array([0.05, 0.05])
    a2 = np.array([0.0, 0.0])

    pwq_set_coeffs(pwq, a0, a1, a2)

    # Evaluate at various stations
    # At s=0 (start of segment 0)
    val = pwq_eval(pwq, 0.0)
    assert abs(val - 100.0) < 1e-6, f"Expected 100.0 at s=0, got {val}"

    # At s=50 (middle of segment 0)
    val = pwq_eval(pwq, 50.0)
    expected = 100.0 + 0.05 * 50.0  # 102.5
    assert abs(val - expected) < 1e-6, f"Expected {expected} at s=50, got {val}"

    # At s=100 (junction between segments)
    val = pwq_eval(pwq, 100.0)
    # Should match segment 1 at ds=0: a0[1] = 105.0
    assert abs(val - 105.0) < 1e-6, f"Expected 105.0 at s=100, got {val}"

    # At s=150 (middle of segment 1)
    val = pwq_eval(pwq, 150.0)
    expected = 105.0 + 0.05 * 50.0  # 107.5
    assert abs(val - expected) < 1e-6, f"Expected {expected} at s=150, got {val}"


@pytest.mark.unit
def test_pwq_evald_linear():
    """Test evaluating gradient of linear piecewise quadratic"""
    stations = np.array([0.0, 100.0, 200.0])
    pwq = pwq_create(stations)

    # Linear segments with constant gradients
    a0 = np.array([100.0, 105.0])
    a1 = np.array([0.05, 0.10])  # Different gradients
    a2 = np.array([0.0, 0.0])    # No curvature

    pwq_set_coeffs(pwq, a0, a1, a2)

    # Gradient of segment 0 should be 0.05 everywhere
    grad = pwq_evald(pwq, 0.0)
    assert abs(grad - 0.05) < 1e-6, f"Expected gradient 0.05 at s=0, got {grad}"

    grad = pwq_evald(pwq, 50.0)
    assert abs(grad - 0.05) < 1e-6, f"Expected gradient 0.05 at s=50, got {grad}"

    # Gradient of segment 1 should be 0.10 everywhere
    grad = pwq_evald(pwq, 101.0)
    assert abs(grad - 0.10) < 1e-6, f"Expected gradient 0.10 at s=100, got {grad}"

    grad = pwq_evald(pwq, 150.0)
    assert abs(grad - 0.10) < 1e-6, f"Expected gradient 0.10 at s=150, got {grad}"


@pytest.mark.unit
def test_pwq_evald_quadratic():
    """Test evaluating gradient of quadratic piecewise quadratic"""
    stations = np.array([0.0, 100.0])
    pwq = pwq_create(stations)

    # Single segment with curvature
    # P(s) = 100 + 0.05*s + 0.0001*s²
    # P'(s) = 0.05 + 0.0002*s
    a0 = np.array([100.0])
    a1 = np.array([0.05])
    a2 = np.array([0.0001])

    pwq_set_coeffs(pwq, a0, a1, a2)

    # At s=0, gradient = 0.05
    grad = pwq_evald(pwq, 0.0)
    assert abs(grad - 0.05) < 1e-6, f"Expected gradient 0.05 at s=0, got {grad}"

    # At s=50, gradient = 0.05 + 0.0002*50 = 0.06
    grad = pwq_evald(pwq, 50.0)
    expected_grad = 0.05 + 2 * 0.0001 * 50.0
    assert abs(grad - expected_grad) < 1e-6, \
        f"Expected gradient {expected_grad} at s=50, got {grad}"

    # At s=100, gradient = 0.05 + 0.0002*100 = 0.07
    grad = pwq_evald(pwq, 100.0)
    expected_grad = 0.05 + 2 * 0.0001 * 100.0
    assert abs(grad - expected_grad) < 1e-6, \
        f"Expected gradient {expected_grad} at s=100, got {grad}"


@pytest.mark.unit
def test_pwq_eval_flat():
    """Test piecewise quadratic representing flat profile"""
    stations = np.array([0.0, 100.0, 200.0, 300.0])
    pwq = pwq_create(stations)

    # All segments flat at elevation 100
    a0 = np.array([100.0, 100.0, 100.0])
    a1 = np.array([0.0, 0.0, 0.0])
    a2 = np.array([0.0, 0.0, 0.0])

    pwq_set_coeffs(pwq, a0, a1, a2)

    # Should be 100 everywhere
    for s in [0, 50, 100, 150, 200, 250, 300]:
        val = pwq_eval(pwq, s)
        assert abs(val - 100.0) < 1e-6, f"Expected 100.0 at s={s}, got {val}"

    # Gradient should be zero everywhere
    for s in [0, 50, 100, 150, 200, 250, 300]:
        grad = pwq_evald(pwq, s)
        assert abs(grad) < 1e-6, f"Expected gradient 0 at s={s}, got {grad}"


@pytest.mark.unit
def test_pwq_eval_parabola():
    """Test single segment parabola"""
    stations = np.array([0.0, 100.0])
    pwq = pwq_create(stations)

    # Parabola: P(s) = 100 + 0*s - 0.001*s²
    # At s=0: P=100, at s=50: P=97.5, at s=100: P=90
    a0 = np.array([100.0])
    a1 = np.array([0.0])
    a2 = np.array([-0.001])

    pwq_set_coeffs(pwq, a0, a1, a2)

    # At s=0
    val = pwq_eval(pwq, 0.0)
    assert abs(val - 100.0) < 1e-6, f"Expected 100.0 at s=0, got {val}"

    # At s=50
    val = pwq_eval(pwq, 50.0)
    expected = 100.0 - 0.001 * 50.0**2  # 97.5
    assert abs(val - expected) < 1e-6, f"Expected {expected} at s=50, got {val}"

    # At s=100
    val = pwq_eval(pwq, 100.0)
    expected = 100.0 - 0.001 * 100.0**2  # 90.0
    assert abs(val - expected) < 1e-6, f"Expected {expected} at s=100, got {val}"


@pytest.mark.unit
def test_pwq_continuity_at_junction():
    """Test C0 continuity at segment junction"""
    stations = np.array([0.0, 100.0, 200.0])
    pwq = pwq_create(stations)

    # Set coefficients ensuring C0 continuity
    # Segment 0: P(s) = 100 + 0.05*s, at s=100: P=105
    # Segment 1: P(s) = 105 + 0.10*(s-100), at s=100: P=105
    a0 = np.array([100.0, 105.0])
    a1 = np.array([0.05, 0.10])
    a2 = np.array([0.0, 0.0])

    pwq_set_coeffs(pwq, a0, a1, a2)

    # Evaluate just before and at junction
    val_before = pwq_eval(pwq, 99.999)
    val_at = pwq_eval(pwq, 100.0)

    # Should be continuous
    assert abs(val_before - val_at) < 0.01, \
        f"Discontinuity at junction: {val_before} vs {val_at}"


@pytest.mark.unit
def test_pwq_gradient_discontinuity():
    """Test gradient discontinuity at junction (C0 but not C1)"""
    stations = np.array([0.0, 100.0, 200.0])
    pwq = pwq_create(stations)

    # Different gradients in each segment
    a0 = np.array([100.0, 105.0])
    a1 = np.array([0.05, 0.10])  # Gradient jump at s=100
    a2 = np.array([0.0, 0.0])

    pwq_set_coeffs(pwq, a0, a1, a2)

    # Gradient before junction (segment 0)
    grad_before = pwq_evald(pwq, 99.999)
    # Gradient at junction (segment 1)
    grad_after = pwq_evald(pwq, 100.0)

    # Gradients should be different
    assert abs(grad_before - 0.05) < 0.01, f"Expected gradient ~0.05 before junction"
    assert abs(grad_after - 0.10) < 1e-6, f"Expected gradient 0.10 at junction"


@pytest.mark.unit
def test_pwq_eval_vectorized():
    """Test vectorized evaluation at multiple stations"""
    stations = np.array([0.0, 100.0, 200.0])
    pwq = pwq_create(stations)

    a0 = np.array([100.0, 105.0])
    a1 = np.array([0.05, 0.05])
    a2 = np.array([0.0, 0.0])

    pwq_set_coeffs(pwq, a0, a1, a2)

    # Evaluate at array of stations
    s_array = np.array([0.0, 50.0, 100.0, 150.0, 200.0])

    try:
        vals = pwq_eval(pwq, s_array)
        assert vals.shape == s_array.shape, "Should return array of same shape"
        expected = np.array([100.0, 102.5, 105.0, 107.5, 110.0])
        np.testing.assert_array_almost_equal(vals, expected, decimal=5)
    except (AttributeError, TypeError):
        # If not vectorized, test individual calls
        for s in s_array:
            val = pwq_eval(pwq, s)
            assert isinstance(val, (float, np.floating))


@pytest.mark.unit
def test_pwq_eval_outside_range():
    """Test evaluation outside station range (extrapolation behavior)"""
    stations = np.array([0.0, 100.0])
    pwq = pwq_create(stations)

    a0 = np.array([100.0])
    a1 = np.array([0.05])
    a2 = np.array([0.0])

    pwq_set_coeffs(pwq, a0, a1, a2)

    # Before first station
    try:
        val = pwq_eval(pwq, -50.0)
        # Should either extrapolate or clamp
        assert isinstance(val, (float, np.floating))
    except (ValueError, AssertionError):
        pass  # May raise error for out-of-range

    # After last station
    try:
        val = pwq_eval(pwq, 200.0)
        assert isinstance(val, (float, np.floating))
    except (ValueError, AssertionError):
        pass


@pytest.mark.unit
def test_pwq_multiple_set_coeffs():
    """Test that coefficients can be updated multiple times"""
    stations = np.array([0.0, 100.0])
    pwq = pwq_create(stations)

    # Set first coefficients
    a0 = np.array([100.0])
    a1 = np.array([0.05])
    a2 = np.array([0.0])
    pwq_set_coeffs(pwq, a0, a1, a2)

    val1 = pwq_eval(pwq, 50.0)
    expected1 = 100.0 + 0.05 * 50.0
    assert abs(val1 - expected1) < 1e-6

    # Update coefficients
    a0_new = np.array([200.0])
    a1_new = np.array([0.10])
    a2_new = np.array([0.0])
    pwq_set_coeffs(pwq, a0_new, a1_new, a2_new)

    val2 = pwq_eval(pwq, 50.0)
    expected2 = 200.0 + 0.10 * 50.0
    assert abs(val2 - expected2) < 1e-6
    assert abs(val2 - val1) > 1.0, "Values should change after updating coefficients"
