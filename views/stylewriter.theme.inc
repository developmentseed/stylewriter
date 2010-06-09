<?php

function template_preprocess_stylewriter_mapfile(&$vars) {
  $view = $vars['view'];

  $points = $view->style_plugin->map_rows($vars['rows']);

  $rows = '';
  foreach ($points as $point) {
    $rows .= theme('stylewriter_rule', $point['value'], $point['join_field']);
  }

  exit($rows);

  $vars['rows'] = $rows;
  $vars['viewtitle'] = $view->name;  

  // drupal_set_header('Content-Type: text/xml');
}
