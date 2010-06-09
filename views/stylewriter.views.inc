<?php
// $Id$

/**
 * @file
 * This file holds main views functions for the stylewriter module.
 *
 * @ingroup stylewriter 
 */

/**
 * Implementation of hook_views_plugins().
 */
function stylewriter_views_plugins() {
  return array(
    'module' => 'stylewriter',
    'display' => array(
      'mapfile' => array(
        'title' => t('Mapfile'),
        'help' => t('A Mapfile display'),
        'handler' => 'stylewriter_plugin_display_mapfile',
        'path' => drupal_get_path('module', 'stylewriter') .'/views',
        'uses hook menu' => TRUE,
        'use ajax' => FALSE,
        'use pager' => FALSE,
        'accept attachments' => FALSE,
        'admin' => t('Stylewriter Mapfile'),
        'type' => 'feed',
        'parent' => 'feed'
      ),
    ),
    'style' => array(
      'mapfile' => array(
        'title' => t('Mapfile'),
        'help' => t(''),
        'handler' => 'views_plugin_style_mapfile',
        'theme' => 'stylewriter_mapfile',
        'theme file' => 'stylewriter.theme.inc',
        'theme path' => drupal_get_path('module', 'stylewriter') .'/views',
        'path' => drupal_get_path('module', 'stylewriter') .'/views',
        'uses fields' => TRUE,
        'uses row plugin' => FALSE,
        'uses options' => TRUE,
        'uses grouping' => TRUE,
        'type' => 'feed',
        'even empty' => TRUE,
      ),
    ),
  );
}

