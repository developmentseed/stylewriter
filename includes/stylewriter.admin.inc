<?php
// $Id$


function stylewriter_get_json_url($url, $params) {
  // Copied from http_request.inc in Feeds
  // TODO: work without curl
  $headers = array('User-Agent: Drupal (+http://drupal.org/)');
  $result = new stdClass();

  $request = curl_init($url);
  curl_setopt($request, CURLOPT_POST, TRUE);
  curl_setopt($request, CURLOPT_POSTFIELDS, urlencode($params));
  curl_setopt($request, CURLOPT_HEADER, TRUE);
  curl_setopt($request, CURLOPT_HTTPHEADER, $headers);
  curl_setopt($request, CURLOPT_RETURNTRANSFER, TRUE);
  $data = curl_exec($request);

  $result->code = curl_getinfo($request, CURLINFO_HTTP_CODE);
  $header_size = curl_getinfo($request, CURLINFO_HEADER_SIZE);
  $header = substr($data, 0, $header_size - 1);
  $result->data = substr($data, $header_size);

  return json_decode($result->data);
}

function stylewriter_cache_clear(&$form, &$form_state) {
  $data_facets = $form_state['values']['cache']['wbapi_cache_data_facets'];
  $mapfile_facts = $form_state['values']['cache']['wbapi_cache_mapfile_facets'];

  $cache_json = stylewriter_get_json_url(
    variable_get('stylewriter_server', '') . 'cache.json',
    array(
      'key' => variable_get('stylewriter_key', ''),
      'data' => $data_facets,
      'mapfile' => $mapfile_facets
    )
  ); 
}

/**
 * Implementation of hook_stylewriter_data_facets()
 */
function stylewriter_stylewriter_data_facets() {
  // TODO: replace with something more robust
  $cache_json = stylewriter_get_json_url(
    variable_get('stylewriter_server', '') . 'cache.json',
    array(
      'key' => variable_get('stylewriter_key', ''))
  );
  return $cache_json->data;
}


/**
 * Implementation of hook_stylewriter_style_facets()
 */
function stylewriter_stylewriter_mapfile_facets() {
  $cache_json = stylewriter_get_json_url(
    variable_get('stylewriter_server', '') . 'cache.json',
    array(
      'key' => variable_get('stylewriter_key', ''))
  );
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

  return system_settings_form($form);
}

function stylewriter_cache_form() {
  $form = array();

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
