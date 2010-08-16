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

  return stylewriter_color_ramp_merge($h_grad, $s_grad, $l_grad);
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
    $c = $a;
    $a = $b;
    $b = $c;
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
  if ($a == $b) {
    return array_fill(0, $steps, $a);
  }
  else {
    return stylewriter_range($a, $b, $steps);
  }
}

/**
 * Merge three arrays representing h, s, l into an array of rgb values
 * @param $h_grad
 * @param $s_grad
 * @param $l_grad
 *
 * @return array
 */
function stylewriter_color_ramp_merge($h_grad, $s_grad, $l_grad) {
  if (count($h_grad) == count($s_grad) && count($s_grad) == count($l_grad)) {
    $colors = array();
    for ($i = 0; $i < count($h_grad); $i++) {
      $colors[] = _color_pack(_color_hsl2rgb(array($h_grad[$i], $s_grad[$i], $l_grad[$i])), TRUE);
    }
    return $colors;
  }
  else {
    throw new Exception('HSL components must have equal length');
  }
}

/**
 * URL-safe base64 encode
 * Compatible with Python's base64.urlsafe_b64decode
 * @param $text base64-decoded text
 */
function stylewriter_safe64en($text) {
  return strtr( 
    chunk_split(
      strtr(
        base64_encode($text), '+/', '-_'), 
      255),
      '\n', '/'
    );
}

/**
 * URL-safe base64 decode 
 * Compatible with Python's base64.urlsafe_b64encode
 * @param $text base64-encoded text
 */
function stylewriter_safe64de($text) {
  return base64_decode(
    strtr(
      str_replace('/', '', $text), '-_', '+/')
    );
}
