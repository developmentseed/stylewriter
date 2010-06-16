<?php
// $Id$


function stylewriter_get_json_url($url) {
  $c = curl_init();
  curl_setopt($c, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($c, CURLOPT_URL, $url);
  $contents = curl_exec($c);
  curl_close($c);

  if ($contents) {
    return json_decode($contents);
  }
}

/**
 * Implementation of hook_stylewriter_data_facets()
 */
function stylewriter_stylewriter_data_facets() {
  // TODO: replace with something more robust
  $cache_json = stylewriter_get_json_url(variable_get('stylewriter_server', '') . 'cache.json');
  return $cache_json->data;
}


/**
 * Implementation of hook_stylewriter_style_facets()
 */
function stylewriter_stylewriter_mapfile_facets() {
  $cache_json = stylewriter_get_json_url(variable_get('stylewriter_server', '') . 'cache.json');
  return $cache_json->mapfile;
}

/**
 * Menu callback; displays the openlayers module settings page.
 *
 * @see system_settings_form()
 */
function stylewriter_settings() {
  $form = array();

  $form['stylewriter_server'] = array(
    '#type' => 'textfield',
    '#title' => t('TileLite Server'),
    '#description' => t('The URL of your TileLite server'),
    '#default_value' => variable_get('stylewriter_server', ''),
  );

  $form['stylewriter_key'] = array(
    '#type' => 'textfield',
    '#title' => t('TileLite server Key'),
    '#description' => t('The shared key which allows you to 
      query and modify the TileLite server'),
    '#default_value' => variable_get('stylewriter_key', ''),
  );

  $form['cache'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cache'),
    '#tree' => FALSE,
  );

  $form['cache']['wbapi_cache_data_facets'] = array(
    '#type' => 'checkboxes',
    '#options' => module_invoke_all('stylewriter_data_facets'),
    '#title' => t('Data Facets'),
  );

  $form['cache']['wbapi_cache_mapfile_facets'] = array(
    '#type' => 'checkboxes',
    '#options' => module_invoke_all('stylewriter_mapfile_facets'),
    '#title' => t('Mapfile Facets'),
  );
  
  $form['cache']['stylewriter_cache_clear'] = array(
    '#type' => 'submit',
    '#value' => t('Clear cache'),
    '#submit' => array('stylewriter_cache_clear'),
  );

  return system_settings_form($form);
}
