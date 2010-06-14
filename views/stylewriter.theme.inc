<?php

function template_preprocess_stylewriter_mapfile(&$vars) {
  $view = $vars['view'];
  $points = $view->style_plugin->legend_data($vars['rows']);

  $swatches = '';
  foreach ($points as $value => $color) {
    $swatches .= theme('stylewriter_swatch', 
      $value,
      $color);
  }
  return theme('stylewriter_legend', $swatches);
 }
