<?php
// $Id$

/**
 * @file
 * StyleWriter utility functions
 */

/**
 * Blend two colors to make a ramp
 * 
 * @param $a
 * @param $b
 * @param $steps
 *
 * @return array
 */
function stylewriter_color_ramp($a, $b, $steps) {
  module_load_include('module', 'color', 'color');

  $a_pts = _color_rgb2hsl(_color_unpack($a, TRUE));
  $b_pts = _color_rgb2hsl(_color_unpack($b, TRUE));

  $h_grad = stylewriter_color_ramp_grad($a_pts[0], $b_pts[0], $steps);
  $s_grad = stylewriter_color_ramp_grad($a_pts[1], $b_pts[1], $steps);
  $l_grad = stylewriter_color_ramp_grad($a_pts[2], $b_pts[2], $steps);

  $merged = array_map(null, $h_grad, $s_grad, $l_grad);

  return stylewriter_color_ramp_merge($merged);
}

/**
 * The built-in PHP range function is a disappointment, so this is a
 * reimplementation based around the requirement that the range should 
 * start and end with $a and $b, respectively, and have $parts elements
 *
 * @param $a start
 * @param $b end
 * @param $parts parts
 */
function stylewriter_range($a, $b, $parts) {
  $range = array();

  // If $a > $b we must flip at the end to preserve direction.
  $reverse = FALSE;
  if ($a > $b) {
    list($a, $b) = array($b, $a);
    $reverse = TRUE;
  }

  $step = (($b - $a) / ($parts - 1));
  for ($i = 0; $i < $parts; $i++) {
    $range[] = $a + ($step * $i);
  }
  return $reverse ? array_reverse($range) : $range;
}

/**
 * Get a ramp between two values that has $steps steps
 * A wrapper around stylewriter_range that ensures that if a==b,
 * it returns an array_fill'ed array of a
 *
 * @param $a
 * @param $b
 * @param $steps
 *
 * @return array
 */
function stylewriter_color_ramp_grad($a, $b, $steps) {
  return ($a == $b) ? 
    array_fill(0, $steps, $a) : 
    stylewriter_range($a, $b, $steps);
}

function _stylewriter_color_pack($c) {
  return _color_pack($c, TRUE);
}

/**
 * Merge three arrays representing h, s, l into an array of rgb values
 * @param $ramps merged groups of color ramps
 *
 * @return array
 */
function stylewriter_color_ramp_merge($ramps) {
  return array_map(
    '_stylewriter_color_pack',
    array_map(
      '_color_hsl2rgb', 
        array_map(
          '_color_hsl2rgb', 
          $ramps
      )
    )
  );
}
