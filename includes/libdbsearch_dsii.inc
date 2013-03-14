<?php

/**
 * @file
 * Drupal Search Interface integration to Libdbsearch module.
 *
 * Last modified: 14.03.2013 Mika Hatakka
 */

/**
 * Implementation of hook_search_info().
 */
function libdbsearch_search_info() {
  return array(
    'title' => 'Library material',
    'path' => 'libdbsearch',
//    'conditions_callback' => 'sample_search_conditions_callback',
  );
}

/**
 * Implementation of hook_search_execute().
 */
function libdbsearch_search_execute($keys = NULL, $conditions = NULL) {
  return libdbsearch_data($keys);
}

/**
 * Implementation of hook_search_page().
 */
function libdbsearch_search_page($results) {
  $output['prefix']['#markup'] = '<ol class="search-results">';

  $results = $results['Hakutulos']['Selauslista']['Teos'];

  foreach ($results as $entry) {
    $output[] = array(
      '#theme' => 'libdbsearch_result',
      '#result' => $entry,
      '#module' => 'libdbsearch',
    );
  }
  $output['suffix']['#markup'] = '</ol>' . theme('pager');

  return $output;
}

/**
 * Implements hook_theme().
 */
function libdbsearch_theme() {
  return array(
    'libdbsearch_result' => array(
      'variables' => array('result' => NULL, 'module' => NULL),
      'file' => 'libdbsearch.pages.inc',
      'template' => 'libdbsearch-result',
    ),
    'libdbsearch_results' => array(
      'variables' => array('results' => NULL, 'module' => NULL),
      'file' => 'libdbsearch.pages.inc',
      'template' => 'libdbsearch-results',
    ),
  );
}
