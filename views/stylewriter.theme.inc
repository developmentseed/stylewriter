<?php
// $Id$

/**
 * @file stylewriter styles
 */

function template_preprocess_stylewriter_mapfile(&$vars) {
  $view = $vars['view'];
  $points = $view->style_plugin->legend_data($vars['rows']);

  $swatches = '';
  $place = 'first';
  foreach ($points as $value => $color) {
    if ($color == end($points)) {
      $place = 'last';
    }
    elseif ($color == reset($points)) {
      $place = 'first';
    }
    else {
      $place = 'mid';
    }
    $swatches .= theme('stylewriter_swatch', 
      $value,
      $color,
      $place);
  }
  return theme('stylewriter_legend', $swatches);
 }
