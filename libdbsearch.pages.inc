<?php

/**
 * @file
 * Page creation function for library database search.
 *
 * Last modified: 14.03.2013 Mika Hatakka
 */

/**
 * Process variables for libdbsearch-results.tpl.php.
 *
 * The $variables array contains the following arguments:
 * - $results: Search results array.
 * - $module: Module the search results came from (module implementing
 *   hook_search_info()).
 *
 * @see libdbsearch-results.tpl.php
 */
function template_preprocess_libdbsearch_results(&$variables) {
  
  dpm($variables, 'results');  
  
  $variables['search_results'] = '';
  if (!empty($variables['module'])) {
    $variables['module'] = check_plain($variables['module']);
  }
  foreach ($variables['results'] as $result) {
    $variables['search_results'] .= theme('libdbsearch_result', array('result' => $result, 'module' => $variables['module']));
  }
  $variables['pager'] = theme('pager', array('tags' => NULL));
  $variables['theme_hook_suggestions'][] = 'libdbsearch_results__' . $variables['module'];
}

/**
 * Process variables for libdbsearch-result.tpl.php.
 *
 * The $variables array contains the following arguments:
 * - $result
 * - $module
 *
 * @see libdbsearch-result.tpl.php
 */
function template_preprocess_libdbsearch_result(&$variables) {
  global $language;

//  dpm($variables, 'result');
  
  $result = $variables['result'];
//  $variables['url'] = check_url($result['link']);
  $variables['url'] = check_url($result['Teoksen_nimi']['Linkki']['Losoite']);
//  $variables['title'] = check_plain($result['title']);
  $variables['title'] = check_plain($result['Teoksen_nimi']['Linkki']['Lteksti']);
//  if (isset($result['language']) && $result['language'] != $language->language && $result['language'] != LANGUAGE_NONE) {
//    $variables['title_attributes_array']['xml:lang'] = $result['language'];
//    $variables['content_attributes_array']['xml:lang'] = $result['language'];
//  }

//  $info = array();
//  if (!empty($result['module'])) {
//    $info['module'] = check_plain($result['module']);
//  }
//  if (!empty($result['user'])) {
//    $info['user'] = $result['user'];
//  }
//  if (!empty($result['date'])) {
//    $info['date'] = format_date($result['date'], 'short');
//  }
//  if (isset($result['extra']) && is_array($result['extra'])) {
//    $info = array_merge($info, $result['extra']);
//  }
//  // Check for existence. User search does not include snippets.
  if (isset($result['Tekija'])) {
    $str = is_array($result['Tekija']) ? '' : check_plain($result['Tekija']);
  }
  $variables['snippet'] = $str;
//  // Provide separated and grouped meta information..
//  $variables['info_split'] = $info;
//  $variables['info'] = implode(' - ', $info);
  $variables['info'] = 
      check_plain($result['Teoksen_aineistolaji']) . ', ' .
      check_plain($result['Julkaisuvuosi']) . ', ' .
      check_plain($result['Luokka']);
  $variables['theme_hook_suggestions'][] = 'libdbsearch_result__' . $variables['module'];
}
