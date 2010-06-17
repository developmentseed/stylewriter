<?php
// $Id$

/**
 * @file
 * StyleWriter module drush integration.
 */

/**
 * Implementation of hook_drush_command().
 * 
 * @See drush_parse_command() for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function stylewriter_drush_command() {
  $items = array();

  $items['stylewriter'] = array(
    'callback' => 'stylewriter_seed',
    'description' => "Seed StyleWriter Tiles",
    'drupal dependencies' => array('stylewriter'),
  );

  return $items;
}

/**
 * Implementation of hook_drush_help().
 */
function stylewriter_drush_help($section) {
  switch ($section) {
    case 'drush:stylewriter':
      return dt("Cache tiles on a TileLite instance");
  }
}

function stylewriter_seed() {
  drush_print(dt("Seeding tiles on a TileLite instance..."));
  
  $data_facets = module_invoke_all('stylewriter_data_possible_facets');

  drush_print(dt("## DATA FACETS"));
  foreach ($data_facets as $facet) {
    drush_print(dt("* !facet", array('!facet' => $facet)));
  }

  $mapfile_facets = module_invoke_all('stylewriter_mapfile_possible_facets');

  drush_print(dt("## MAPFILE FACETS"));
  foreach ($mapfile_facets as $facet) {
    drush_print(dt("* !facet", array('!facet' => $facet)));
  }

  if (count($data_facets) > 1) {
    exit('Only one data facet is currently supported');
  }

  foreach ($mapfile_facets as $facet) {
    $mapfile = $facet;
    $data = $data_facets[0];
    $url = 'http://localhost:8000/';
    _drush_proc_open('./tilelite-seed.py ' .
      '--url="' . $url . '" ' .
      '--data="' . $data . '" ' .
      '--mapfile="' . $mapfile . '" ' .
      '0 1 2 3 &');
  }
}
