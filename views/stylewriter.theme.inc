<?php

function template_preprocess_stylewriter_mapfile(&$vars) {
  $view = $vars['view'];
  if (isset($view->args['legend'])) {
    $points = $view->style_plugin->map_rows($vars['rows']);

    $swatches = '';
    foreach ($points as $point) {
      $swatches .= theme('stylewriter_swatch', 
        $point['value'],
        $point['color']);
    }

    $vars['swatches'] = $rows;
    $vars['viewtitle'] = $view->name;  
    exit(theme('stylewriter_legend', $rows));
  }
  else {
    $points = $view->style_plugin->map_rows($vars['rows']);

    $rows = '';
    foreach ($points as $point) {
      $rows .= theme('stylewriter_rule', 
        $point['join_field_name'],
        $point['join_field_value'],
        $point['color']);
    }
    $vars['rows'] = $rows;
    $vars['viewtitle'] = $view->name;  
    exit(theme('stylewriter_document', $rows));
  }
}
