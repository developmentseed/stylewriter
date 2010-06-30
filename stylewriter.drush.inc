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

  $items['stylewriter-cache-all'] = array(
    'callback' => 'stylewriter_seed',
    'description' => "Seed StyleWriter Tiles",
    'options' => array(
      'tileserver_url' => 'Base URL of the TileLive server'
    ),
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
  if ($tileserver_url = drush_get_option('tileserver_url')) {
    drush_print(dt("Seeding tiles on a TileLite instance..."));
    
    $data_facets = module_invoke_all('stylewriter_data_possible_facets');

    /*
    drush_print(dt("## DATA FACETS"));
    foreach ($data_facets as $facet) {
      drush_print(dt("* !facet", array('!facet' => $facet)));
    }
     */

    $mapfile_facets = module_invoke_all('stylewriter_mapfile_possible_facets');


    /*
    drush_print(dt("## MAPFILE FACETS"));
    foreach ($mapfile_facets as $facet) {
      drush_print(dt("* !facet", array('!facet' => $facet)));
    }
     */

    if (count($data_facets) > 1) {
      exit('Only one data facet is currently supported');
    }

    foreach ($mapfile_facets as $facet) {
      drush_print(dt('Caching !facet', array('!facet' => $facet)));
      $mapfile = $facet;
      $data = $data_facets[0];
      $cmd = 'tileseed.py ' .
        '--url="' . $tileserver_url . '" ' .
        '--data="' . $data . '" ' .
        '--extension="' . 'png' . '" ' .
        '--mapfile="' . $mapfile . '" ' .
        '0 2';
      drush_print('Running ' . $cmd);
      _drush_proc_open($cmd);
    }
  }
  else {
    drush_print('--tileserver_url required');
  }
}
