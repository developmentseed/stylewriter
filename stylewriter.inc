<?php

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

  $a_pts = _color_rgb2hsl(_color_unpack($a));
  $b_pts = _color_rgb2hsl(_color_unpack($b));

  $h_grad = stylewriter_color_ramp_grad($a_pts[0], $b_pts[0], $steps);
  $s_grad = stylewriter_color_ramp_grad($a_pts[1], $b_pts[1], $steps);
  $l_grad = stylewriter_color_ramp_grad($a_pts[2], $b_pts[2], $steps);

  return stylewriter_color_ramp_merge($h_grad, $s_grad, $l_grad);
}

/**
 * Get a ramp between two values that has $steps steps
 * @param $a
 * @param $b
 * @param $steps
 *
 * @return array
 */
function stylewriter_color_ramp_grad($a, $b, $steps) {
  if ($a == $b) {
    return array_fill(0, $steps + 1, $a);
  }
  else {
    $range = $b - $a; // range can be negative
    $step = $range / $steps;
    return range($a, $b, $step);
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
      $colors[] = _color_pack(_color_hsl2rgb(array($h_grad[$i], $s_grad[$i], $l_grad[$i])));
    }
    return $colors;
  }
  else {
    throw new Exception('HSL components must have equal length');
  }
}

/**
 * URL-safe base64 encode
 * @param $text base64-decoded text
 */
function stylewriter_base64en($text) {
  return strtr(base64_encode($text), '+/=', '-_,');
}

/**
 * URL-safe base64 decode 
 * @param $text base64-encoded text
 */
function stylewriter_base64de($text) {
  return base64_decode(strtr($text, '-_,', '+/='));
}
