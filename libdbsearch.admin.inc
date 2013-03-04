<?php

// $Id$

/**
 * @file
 * Libdbsearch settings form.
 *
 * Last modified: 03.08.2011 Mika Hatakka
 */

/**
 * Set fields for module settings, shown to adminitrator.
 *
 * @return array field description for settings
 */
function libdbsearch_admin() {
  $form = array();
  $form['libdb_server'] = array(
    '#type' => 'textfield',
    '#title' => t('Address of used database'),
    '#default_value' => variable_get('libdb_server', ''),
    '#description' => t("Address where all database querys must send."),
    '#required' => TRUE,
  );
//  $form['libdbsearch_subsite'] = array(
//    '#type' => 'textfield',
//    '#title' => t('Sub site folder'),
//    '#default_value' => variable_get('libdbsearch_subsite', ''),
//    '#description' => t("If site is subfolder, the folder name. If in root level leave blank."),
//    '#required' => FALSE,
//  );
  $available = array('undefined');
  $moduulit = module_list();
  foreach ($moduulit as $val) {
    if (module_hook($val, 'libsearch')) {
      $available[$val] = $val;
    }
  }
  $form['libdbsearch_engine'] = array(
    '#title' => t('Chosen search engine module'),
    '#type' => 'select',
    '#options' => $available,
    '#default_value' => variable_get('libdbsearch_engine', '0'),
  );
  $form['libdbsearch_extra_info'] = array(
    '#title' => t('Extra information'),
    '#type' => 'textarea',
    '#description' => t('Write here your text to show in at the bottom of block.'),
    '#rows' => 3,
    '#default_value' => variable_get('libdbsearch_extra_info', ''),
  );
//  $filters = filter_formats();
//  foreach ($filters as $val) {
//    $f_filters[$val->format] = $val->name;
//  }
//  $form['libsearch_format'] = array(
//    '#title' => t('Select input format'),
//    '#type' => 'select',
//    '#description' => (t('Select format used in additional settings.')),
//    '#options' => $f_filters,
//    '#default_value' => variable_get('libsearch_format', 0),
//  );
  return system_settings_form($form);
}

function libdbsearch_admin_validate($form, &$form_state) {
  if(isset($form_state['values']['libdb_server'])) {
    $srv = $form_state['values']['libdb_server'];
    if( preg_match('/^http:\/\/\s*([^;]*)/mi', $srv) ||
        preg_match('/^https:\/\/\s*([^;]*)/mi', $srv) ) {
      if (substr($srv, -1) != '/') {
        $form_state['values']['libdb_server'] = $srv . '/';
      }
    }
    else {
      form_set_error('URL', t('Give full URL address.'));
    }
  }
}
