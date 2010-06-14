<?php
// $Id$

/**
 * Implementation of hook_stylewriter_data_facets()
 */
function stylewriter_stylewriter_data_facets() {
  $facets = array();

  if (module_exists('openlayers')) {
    module_load_include('module', 'openlayers', 'openlayers');
    $layers = openlayers_layers_load();
    foreach ($layers as $layer) {
      if ($layer->data['layer_type'] == 'openlayers_layer_type_stylewriter') {
        $facets[] = $layer->data['data_url'];
      }
    }
  }

  return $facets;
}

/**
 * Menu callback; displays the openlayers module settings page.
 *
 * @see system_settings_form()
 */
function stylewriter_settings() {
  $form = array();
  // Define Form
  $form['stylewriter_server'] = array(
    '#type' => 'textfield',
    '#title' => t('StyleWriter Server'),
    '#description' => t('The URL of your StyleWriter server'),
    '#default_value' => variable_get('stylewriter_server', ''),
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
  $form['cache']['wbapi_cache_style_facets'] = array(
    '#type' => 'checkboxes',
    '#options' => module_invoke_all('stylewriter_style_facets'),
    '#title' => t('Style Facets'),
  );
  $form['cache']['stylewriter_cache_clear'] = array(
    '#type' => 'submit',
    '#value' => t('Clear cache'),
    '#submit' => array('stylewriter_cache_clear'),
  );

  // Make a system setting form and return
  return system_settings_form($form);
}
