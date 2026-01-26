import pytest
import numpy as np
from areafit import areafit_cut, areafit_fill
from crosssection import xs_cut_areas, xs_fill_areas
from groundprofile import gp_create
from pwquadratic import pwq_set_coeffs
from vertprofile import vp_create


@pytest.mark.unit
def test_areafit_cut_basic():
    """Test cut area fitting returns correct structure"""
    # Create flat ground and vertical profile
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

    # Sample offsets from -4 to 0
    u_samples = np.linspace(-4.0, 0.0, 20)

    # Get cut areas at station 50m
    v_samples = xs_cut_areas(vp, gp, 50.0, u_samples)

    # Fit quadratic
    coeff, u_offset, rmserror = areafit_cut(u_samples, v_samples)

    # Check return types
    assert isinstance(coeff, np.ndarray), f"Expected ndarray for coeff, got {type(coeff)}"
    assert len(coeff) == 3, f"Expected 3 coefficients, got {len(coeff)}"
    assert isinstance(u_offset, (float, np.floating)), f"Expected float for u_offset, got {type(u_offset)}"
    assert isinstance(rmserror, (float, np.floating)), f"Expected float for rmserror, got {type(rmserror)}"

    # Coefficients are [c, b, a] for polynomial c + b*u + a*u²
    c, b, a = coeff

    # RMS error should be small for good fit
    assert a > 0, f"Quadratic component should be positive"
    assert rmserror < 1.0, f"Expected small RMS error, got {rmserror}"


@pytest.mark.unit
def test_areafit_fill_basic():
    """Test fill area fitting returns correct structure"""
    # Create flat ground and vertical profile
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

    # Sample offsets from 0 to 4
    u_samples = np.linspace(0.0, 4.0, 20)

    # Get fill areas at station 50m
    v_samples = xs_fill_areas(vp, gp, 50.0, u_samples)

    # Fit quadratic
    coeff, u_offset, rmserror = areafit_fill(u_samples, v_samples)

    # Check return types
    assert isinstance(coeff, np.ndarray), f"Expected ndarray for coeff, got {type(coeff)}"
    assert len(coeff) == 3, f"Expected 3 coefficients, got {len(coeff)}"
    assert isinstance(u_offset, (float, np.floating)), f"Expected float for u_offset, got {type(u_offset)}"
    assert isinstance(rmserror, (float, np.floating)), f"Expected float for rmserror, got {type(rmserror)}"

    # RMS error should be small for good fit
    assert rmserror < 1.0, f"Expected small RMS error, got {rmserror}"


@pytest.mark.unit
def test_areafit_cut_fit_quality():
    """Test that cut area fit has good quality"""
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

    u_samples = np.linspace(-4.0, 0.0, 20)
    v_samples = xs_cut_areas(vp, gp, 50.0, u_samples)

    coeff, u_offset, rmserror = areafit_cut(u_samples, v_samples)
    c, b, a = coeff

    # Verify fit quality by comparing fitted values to true values
    max_error = 0.0
    for u, v_true in zip(u_samples, v_samples):
        v_fit = c + b*u + a*u**2
        if v_true > 1e-6:  # Only check non-zero areas
            rel_error = abs(v_fit - v_true) / v_true
            max_error = max(max_error, rel_error)

    assert max_error < 0.2, f"Fit quality poor: max relative error = {max_error}"


@pytest.mark.unit
def test_areafit_fill_fit_quality():
    """Test that fill area fit has good quality"""
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

    u_samples = np.linspace(0.0, 4.0, 20)
    v_samples = xs_fill_areas(vp, gp, 50.0, u_samples)

    coeff, u_offset, rmserror = areafit_fill(u_samples, v_samples)
    c, b, a = coeff

    # Verify fit quality
    max_error = 0.0
    for u, v_true in zip(u_samples, v_samples):
        v_fit = c + b*u + a*u**2
        if v_true > 1e-6:  # Only check non-zero areas
            rel_error = abs(v_fit - v_true) / v_true
            max_error = max(max_error, rel_error)

    assert max_error < 0.2, f"Fit quality poor: max relative error = {max_error}"


@pytest.mark.unit
def test_areafit_cut_varying_side_slope():
    """Test cut area fitting for different side slopes"""
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))

    u_samples = np.linspace(-4.0, 0.0, 20)

    for side_slope in [1.0, 2.0, 3.0, 4.0]:
        vp = vp_create(np.array([0.0, 100.0]), side_slope=side_slope, road_width=6.0)
        pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

        v_samples = xs_cut_areas(vp, gp, 50.0, u_samples)

        coeff, u_offset, rmserror = areafit_cut(u_samples, v_samples)

        # Should have reasonable fit quality
        assert rmserror < 2.0, f"RMS error too large for side_slope={side_slope}: {rmserror}"


@pytest.mark.unit
def test_areafit_fill_varying_road_width():
    """Test fill area fitting for different road widths"""
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))

    u_samples = np.linspace(0.0, 4.0, 20)

    for road_width in [3.5, 6.0, 10.0, 12.0]:
        vp = vp_create(np.array([0.0, 100.0]), side_slope=2.0, road_width=road_width)
        pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

        v_samples = xs_fill_areas(vp, gp, 50.0, u_samples)

        coeff, u_offset, rmserror = areafit_fill(u_samples, v_samples)

        # Should have reasonable fit quality
        assert rmserror < 2.0, f"RMS error too large for road_width={road_width}: {rmserror}"


@pytest.mark.unit
def test_areafit_cut_filters_zero_areas():
    """Test that areafit_cut filters out zero areas (positive offsets)"""
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

    # Include both negative (cut) and positive (no cut) offsets
    u_samples = np.linspace(-4.0, 4.0, 40)
    v_samples = xs_cut_areas(vp, gp, 50.0, u_samples)

    # Should still fit successfully (filters out v=0 regions internally)
    coeff, u_offset, rmserror = areafit_cut(u_samples, v_samples)

    # Check that fit is valid
    assert isinstance(coeff, np.ndarray)
    assert len(coeff) == 3


@pytest.mark.unit
def test_areafit_fill_filters_zero_areas():
    """Test that areafit_fill filters out zero areas (negative offsets)"""
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

    # Include both negative (no fill) and positive (fill) offsets
    u_samples = np.linspace(-4.0, 4.0, 40)
    v_samples = xs_fill_areas(vp, gp, 50.0, u_samples)

    # Should still fit successfully (filters out v=0 regions internally)
    coeff, u_offset, rmserror = areafit_fill(u_samples, v_samples)

    # Check that fit is valid
    assert isinstance(coeff, np.ndarray)
    assert len(coeff) == 3


@pytest.mark.unit
def test_areafit_cut_coefficients_evaluation():
    """Test that coefficients can be used to evaluate areas"""
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

    u_samples = np.linspace(-4.0, 0.0, 20)
    v_samples = xs_cut_areas(vp, gp, 50.0, u_samples)

    coeff, u_offset, rmserror = areafit_cut(u_samples, v_samples)
    c, b, a = coeff

    # Test at a specific offset
    u_test = -2.0
    v_test_true = xs_cut_areas(vp, gp, 50.0, np.array([u_test]))[0]
    v_test_fit = c + b*u_test + a*u_test**2

    # Should be reasonably close
    rel_error = abs(v_test_fit - v_test_true) / (v_test_true + 1e-10)
    assert rel_error < 0.2, f"Fit evaluation error at u={u_test}: {rel_error}"


@pytest.mark.unit
def test_areafit_fill_coefficients_evaluation():
    """Test that coefficients can be used to evaluate areas"""
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

    u_samples = np.linspace(0.0, 4.0, 20)
    v_samples = xs_fill_areas(vp, gp, 50.0, u_samples)

    coeff, u_offset, rmserror = areafit_fill(u_samples, v_samples)
    c, b, a = coeff

    # Test at a specific offset
    u_test = 2.0
    v_test_true = xs_fill_areas(vp, gp, 50.0, np.array([u_test]))[0]
    v_test_fit = c + b*u_test + a*u_test**2

    # Should be reasonably close
    rel_error = abs(v_test_fit - v_test_true) / (v_test_true + 1e-10)
    assert rel_error < 0.2, f"Fit evaluation error at u={u_test}: {rel_error}"


@pytest.mark.unit
def test_areafit_cut_with_few_samples():
    """Test cut area fitting with minimal samples"""
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

    # Only 5 samples
    u_samples = np.linspace(-4.0, -0.5, 5)
    v_samples = xs_cut_areas(vp, gp, 50.0, u_samples)

    coeff, u_offset, rmserror = areafit_cut(u_samples, v_samples)

    # Should still work
    assert isinstance(coeff, np.ndarray)
    assert len(coeff) == 3


@pytest.mark.unit
def test_areafit_fill_with_many_samples():
    """Test fill area fitting with many samples"""
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

    # Many samples (50)
    u_samples = np.linspace(0.5, 4.0, 50)
    v_samples = xs_fill_areas(vp, gp, 50.0, u_samples)

    coeff, u_offset, rmserror = areafit_fill(u_samples, v_samples)

    # Should have good fit with many samples
    assert rmserror < 1.0, f"Expected good fit with many samples, got rmserror={rmserror}"


@pytest.mark.unit
def test_areafit_cut_fill_symmetry():
    """Test that cut and fill fits have similar characteristics for symmetric cross-section"""
    gp = gp_create(np.array([0.0, 100.0]), np.array([100.0, 100.0]))
    vp = vp_create(np.array([0.0, 100.0]), side_slope=2.0, road_width=6.0)
    pwq_set_coeffs(vp.pwq, np.array([100.0]), np.array([0.0]), np.array([0.0]))

    # Symmetric offset ranges
    u_cut = np.linspace(-4.0, -0.5, 20)
    u_fill = np.linspace(0.5, 4.0, 20)

    v_cut = xs_cut_areas(vp, gp, 50.0, u_cut)
    v_fill = xs_fill_areas(vp, gp, 50.0, u_fill)

    coeff_cut, u_offset_cut, rmserror_cut = areafit_cut(u_cut, v_cut)
    coeff_fill, u_offset_fill, rmserror_fill = areafit_fill(u_fill, v_fill)

    # RMS errors should be similar for symmetric cross-section
    # (though not necessarily identical due to numerical fitting)
    assert abs(rmserror_cut - rmserror_fill) < 5.0, \
        f"Cut and fill fits should have similar quality: cut={rmserror_cut}, fill={rmserror_fill}"
