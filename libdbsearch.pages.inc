<?php
/**
 * @file
 * Search result templates for Drupal page.
 *
 * Last modified: 18.04.2013 Mika Hatakka
 */
// Origo librarysytem specific extractor class.
require_once('includes/search_from_origo.php');

/**
 * Page frame for material search results.
 *
 * @param mixed $variables data from which page is created
 */
function template_preprocess_libdbsearch_materials(&$variables) {
  $variables['libdbsearch_materials'] = '';
  $origo = $variables['results'];
  if (!empty($origo)) {
    $variables['page_title'] = t('Search result'); //t('!hits', array('!hits' => $origo->Otsikko));
    $my_ret = array();
    if (preg_match_all("/[0-9]+/", $origo->Hakuinfo->Teksti, $my_ret) !== FALSE) {
      $variables['hits'] = t('There were !hits hits.', array('!hits' => $my_ret[0][0]));
    }
    else {
      $variables['hits'] = t($origo->Hakuinfo->Teksti);
    }
    if (isset($origo->Lohko->Linkki->Lteksti) && $origo->Lohko->Linkki->Lteksti == 'Näytä kaikki') {
      $variables['show_all'] = _link_from_lobj($origo->Lohko->Linkki, $origo->sid);
    }
    foreach ($origo->Hakutulos->Aineistolajittain->Aineistolaji as $result) {
      $result->addChild('sid', $origo->sid);
      $variables['libdbsearch_materials'] .= theme('libdbsearch_material', $result, $variables['type']);
    }
    //  $variables['pager'] = theme('pager', NULL, 10, 0);
  }
  // Provide alternate search results template.
  $variables['template_files'][] = 'libdbsearch-materials-' . $variables['type'];
}

/**
 * Creates one row in material result page
 *
 * @param <type> $variables result row data
 */
function template_preprocess_libdbsearch_material(&$variables) {
  $result = $variables['result'];

  $variables['found_material'] = _add_link_or_text($result->Laji, $result->sid);
  $variables['found_count'] = (string) $result->Maara;

  // Provide alternate search result template.
  $variables['template_files'][] = 'libdbsearch-material-' . $variables['type'];
}

/**
 * Create's frame for search result data.
 *
 * @param array $variables data to shown in page
 */
function template_preprocess_libdbsearch_browses(&$variables) {
  $variables['libdbsearch_browses'] = '';
  $origo = $variables['results'];
  if (!empty($origo)) {
    $variables['origo_title'] = t('Search result');
    //$variables['origo_title'] = t('!otsikko', array('!otsikko' => $origo->Otsikko));
    if (!empty($origo->Hakutulos->Selauslista->Teos)) {
      $variables['hit_info'] = '';
      if (isset($origo->Hakuinfo->Teksti)) {
        foreach ($origo->Hakuinfo->Teksti as $val) {
          if (!empty($variables['hit_info'])) {
            $variables['hit_info'] .= '<br />';
          }
          $my_ret = NULL;
          $my_cnt = preg_match_all("/[0-9]+/", $val, $my_ret);
          if ($my_cnt == 1) {
            $variables['hit_info'] .= t('There were !hits hits.', array('!hits' => $my_ret[0][0]));
          }
          elseif ($my_cnt == 2) {
            if ($my_ret[0][0] > $my_ret[0][1]) { // Add to handle case where more than 1000 results.
              $variables['hit_info'] .= t('There were !hits hits.', array('!hits' => $my_ret[0][0]));
              $variables['hit_info'] .= t(' The first !hits are shown.', array('!hits' => $my_ret[0][1]));
            }
            else {
              $variables['hit_info'] .= t('Works !hit1 - !hit2', array('!hit1' => $my_ret[0][0], '!hit2' => $my_ret[0][1]));
            }
          }
        }
      }
      foreach ($origo->Hakutulos->Selausotsikot->Totsikko as $t) {
        if (isset($t->Linkki)) {
          $variables['list_capt'][(int) $t->Paikka] = _link_from_lobj($t->Linkki, $origo->sid);
        }
        else {
          $variables['list_capt'][(int) $t->Paikka] = libdb_t((string) $t->Teksti);
        }
      }
      $variables['navi0'] = _add_navi($origo->Navigointi[0], $origo->sid);

      foreach ($origo->Hakutulos->Selauslista->Teos as $t) {
        $t->addChild('sid', $origo->sid);
        $variables['libdbsearch_browses'] .=
            theme('libdbsearch_browse', $t, $variables['type']);
      }

      //  $variables['pager'] = theme('pager', NULL, 10, 0);
      if ($origo->Lohko->Linkki->Lteksti == 'Aineistolajeittain ryhmittely') {
        $variables['navi_back'] = _link_from_lobj($origo->Lohko->Linkki, $origo->sid);
      }
      $variables['navi1'] = _add_navi($origo->Navigointi[1], $origo->sid);
    }
    else {
      $variables['hit_info'] = 'No results';
      $variables['no_results'] = 'true';
    }
  }
  // Provide alternate search results template.
  $variables['template_files'][] = 'libdbsearch-browses-' . $variables['type'];
}

/**
 * Create's one table row from search result data.
 *
 * @param array $variables data to show i table rows
 */
function template_preprocess_libdbsearch_browse(&$variables) {
  $result = $variables['result'];
  $variables['found_author'] = _get_value($result->Tekija);

  if (isset($result->Teoksen_nimi->Linkki)) {
    $variables['found_title'] = _link_from_lobj($result->Teoksen_nimi->Linkki, $result->sid);
  }
  else {
    $variables['found_title'] = $result->Teoksen_nimi;
  }
  $variables['found_material'] = _get_value($result->Teoksen_aineistolaji);
  $variables['found_year'] = _get_value($result->Julkaisuvuosi);
  $variables['found_class'] = _get_value($result->Luokka);
  // Provide alternate search result template.
  $variables['template_files'][] = 'libdbsearch-material-' . $variables['type'];
}

/**
 * Creates the work information page.
 *
 * @param <type> $variables data to shown in page
 */
function template_preprocess_libdbsearch_works(&$variables) {
  $variables['libdbsearch_works'] = '';
  $variables['libdbsearch_avail'] = '';
  $variables['libdbsearch_libray'] = '';
  $origo = $variables['results'];
  if (!empty($origo)) {
    $variables['title'] = t('Work information');
    $variables['copylink'] = _link_from_text('teos/?teosid=' . $_GET['teosid'], t('Copy link'));
    if (isset($origo->Kuva)) {
      $variables['picture'] = '<img class="work_pic" border="0" width="100" alt="Teoksen kuva" src="' . $origo->Kuva . '">';
      //$variables['picture'] = '<img class="work_pic" border="0" alt="Teoksen kuva" src="'.$origo->Kuva.'">';
    }
    if (!empty($origo->Teostiedot)) {
      foreach ($origo->Teostiedot->Rivi as $val) {
        $val->addChild('sid', $origo->sid);
        $variables['libdbsearch_works'] .= theme('libdbsearch_work', $val, $variables['type']);
        if (((string) $val->Selite) == 'Teoksen nimi') {
          if (($vinkki = is_vinkki((string) $val->Tieto->Teksti)) !== FALSE) {
            $variables['vinkki_linkki'] = $vinkki;
          }
        }
      }
      if (isset($origo->Osantiedot)) {
        $val = new SimpleXMLElement('<Tieto></Tieto>');
        $val->addChild('Selite', '<strong>' . t('Section information') . '</strong>');
        $variables['libdbsearch_works'] .= theme('libdbsearch_work', $val, $variables['type']);
        foreach ($origo->Osantiedot->Rivi as $val) {
          $val->addChild('sid', $origo->sid);
          $variables['libdbsearch_works'] .= theme('libdbsearch_work', $val, $variables['type']);
        }
      }
      if (isset($origo->Navigointi[0])) {
        $variables['navi_top'] = '';
        foreach ($origo->Navigointi[0] as $l) {
          if (isset($l->Linkki)) {
            $variables['navi_top'] .= _link_from_lobj($l->Linkki, $origo->sid);
          }
          else {
            $variables['navi_top'] .= libdb_t($l);
          }
          $variables['navi_top'] .= '&nbsp;&nbsp;';
        }
      }
      if (isset($origo->Navigointi[1]->Valinta[1])) {
        $variables['navi_back'] = _link_from_lobj($origo->Navigointi[1]->Valinta[1]->Linkki, $origo->sid);
      }
      if (isset($origo->Lohko)) {
        foreach ($origo->Lohko as $l) {
          if ($l->Linkki->Lteksti == 'Varaa teos') {
            $res_addr = variable_get('libdbsearch_reserve', '');
            if (empty($res_addr)) {
              $variables['varaa_teos'] = '<a href="http://' .
                  variable_get('libdbsearch_db_server', '') . '/index.asp?url=' .
                  $l->Linkki->Losoite;
              if (isset($l->Linkki->Lnewwnd)) {
                $variables['varaa_teos'] .= '" target="_blank';
              }
              $variables['varaa_teos'] .= '">' . libdb_t($l->Linkki->Lteksti) . '</a>';
            }
            else {
              if (strpos($res_addr, 'http') === FALSE) {
                $variables['varaa_teos'] = l(
                    libdb_t($l->Linkki->Lteksti), $res_addr . $l->Linkki->Losoite);
              }
              else {
                $variables['varaa_teos'] = '<a href="' . $res_addr . $l->Linkki->Losoite;
                $variables['varaa_teos'] .= '">' . libdb_t($l->Linkki->Lteksti) . '</a>';
              }
            }
          }
          elseif ($l->Linkki->Lteksti == 'Tarkat tiedot') {
            $variables['tarkat_tiedot'] = _link_from_lobj($l->Linkki, $origo->sid);
          }
          elseif ($l->Linkki->Lteksti == 'Lainaa sähköinen kirja') {
            $variables['ebook_loan'] = '<div class="eaineisto">';
            $variables['ebook_loan'] .= '<span class="linkki">';
            $eb_link = '<a href=http://' .
                    variable_get('libdbsearch_db_server', '') . '/' . 
                    $l->Linkki->Losoite;
            if (isset($l->Linkki->Lnewwnd)) {
              $eb_link .= ' target="_blank" ';
            }
            $variables['ebook_loan'] .= $eb_link . '>' . 
                    libdb_t($l->Linkki->Lteksti) . '</a></span>';
            $variables['ebook_loan'] .= $eb_link . '>' . 
                    '<img src="http://kyyti.verkkokirjasto.fi/kuvat/ewelib2_70x32.png" />' . 
                    '</a>';
            $variables['ebook_loan'] .= '</div>';
          }
          elseif ($l->Linkki->Lteksti == 'Saatavuustiedot') {
            $variables['saatavuustiedot'] = _link_from_lobj($l->Linkki, $origo->sid);
          }
          elseif ($l->Linkki->Lteksti == 'Esittelyteksti') {
            $variables['esittelyteksti'] = '<a href=' .
                $l->Linkki->Losoite;
            if (isset($l->Linkki->Lnewwnd)) {
              $variables['esittelyteksti'] .= ' target="_blank" ';
            }
            $variables['esittelyteksti'] .= '>' . libdb_t($l->Linkki->Lteksti) . '</a>';
          }
          elseif ($l->Linkki->Lteksti == 'Näytä sisältö / kappaleet') {
            $variables['nayta_sisalto'] = _link_from_lobj($l->Linkki, $origo->sid);
          }
        }
      }
      //  $variables['pager'] = theme('pager', NULL, 10, 0);
      if (isset($origo->Saatavuus)) {
        $variables['valiotsikko'] = libdb_t($origo->Saatavuus->Valiotsikko);
        $variables['valiteksti'] = $origo->Saatavuus->Teksti;
        foreach ($origo->Saatavuus->Teos as $t) {
          $t->addChild('sid', $origo->sid);
          $variables['libdbsearch_avail'] .=
              theme('libdbsearch_avail', $t, $variables['type']);
        }
      }
      if (isset($origo->Valitun_saatavuus)) {
        $variables['kirjastonotsikko'] = $origo->Valitun_saatavuus->Valiotsikko;
        foreach ($origo->Valitun_saatavuus->Kirjasto as $result) {
          $variables['libdbsearch_library'] .=
              theme('libdbsearch_library', $result, $variables['type']);
        }
      }
      if (empty($variables['vinkki_linkki'])) {
        $variables['lisaa_node'] = libdbsearch_node_add_link(
            $origo->Teostiedot, 'teos/?teosid=' . $_GET['teosid']);
      }
    }
    else {
      $variables['hit_info'] = 'No results';
      $variables['no_results'] = 'true';
    }
  }
  // Provide alternate search results template.
  $variables['template_files'][] = 'libdbsearch-works-' . $variables['type'];
}

/**
 * Shows work related information on page.
 *
 * @param <type> $variables work information
 * @return void return if no data to show
 */
function template_preprocess_libdbsearch_work(&$variables) {
  $result = $variables['result'];
  $variables['caption_item'] = libdb_t($result->Selite);
  $variables['list_item'] = _make_var($result->Tieto, $result->sid);
  // Provide alternate search result template.
  $variables['template_files'][] = 'libdbsearch-work-' . $variables['type'];
}

/**
 * Availebility information in libraries.
 *
 * @param <type> $variables data to shown in page
 */
function template_preprocess_libdbsearch_avail(&$variables) {
  $result = $variables['result'];
  $variables['a_collection'] = _add_link_or_text($result->Kokoelma, $result->sid);
  $variables['a_items'] = $result->Niteita;
  $variables['a_borrow'] = $result->Lainattavissa;
  $variables['a_ordered'] = $result->Tilattu;
  // If one of books can be borrowed, no need to show due day in this view.
  if ((int) $result->Lainattavissa > 0) {
    $variables['a_due'] = '-';
  }
  else {
    $variables['a_due'] = (string) $result->Erapaiva;
  }
  //$variables['a_due'] = $result->Erapaiva;
  // Provide alternate search result template.
  $variables['template_files'][] = 'libdbsearch-avail-' . $variables['type'];
}

/**
 * Creates one library region libraries
 * availability information page.
 *
 * @param <type> $variables one library avail information
 */
function template_preprocess_libdbsearch_library(&$variables) {
  $result = $variables['result'];
  $variables['l_library'] = $result->Kirjastonimi;
  foreach ($result->Sijainti as $loc) {
    $apu['l_location'] = (string) $loc->Osasto;
    $apu['l_shelf'] = (string) $loc->Hylly;
    $apu['l_class'] = (string) $loc->Luokka;
    $apu['l_items'] = (string) $loc->Niteita;
    $apu['l_borrow'] = (string) $loc->Lainattavissa;
    $apu['l_ordered'] = (string) $loc->Tilattu;
    $apu['l_due'] = (string) $loc->Erapaiva;
    $variables['department'][] = $apu;
  }
  // Provide alternate search result template.
  $variables['template_files'][] = 'libdbsearch-library-' . $variables['type'];
}

/**
 * Creates the work's sub object information page.
 *
 * @param <type> $variables data to shown in page
 */
function template_preprocess_libdbsearch_partworks(&$variables) {
  $variables['libdbsearch_partworks'] = '';
  $subpart = $variables['results'];
  if (isset($subpart->Otsikko->Linkki)) {
    $variables['title'] = _link_from_lobj($subpart->Otsikko->Linkki, $subpart->sid);
  }
  else {
    $variables['title'] = $subpart->Otsikko;
  }
  if (!empty($subpart->Osakohteet)) {
    $variables['part_info'] = extract_teksti($subpart->Teksti);
    foreach ($subpart->Osakohteet->Totsikot->Totsikko as $t) {
      if (isset($t->Linkki)) {
        $variables['list_capt'][(int) $t->Paikka] = _link_from_lobj($t->Linkki, $subpart->sid);
      }
      else {
        $variables['list_capt'][(int) $t->Paikka] = libdb_t((string) $t->Teksti);
      }
    }
    foreach ($subpart->Osakohteet->Rivi as $row) {
      $row->addChild('sid', $subpart->sid);
      $variables['libdbsearch_partworks'] .= theme('libdbsearch_partwork', $row, $variables['type']);
    }
    $variables['navi_back'] = _link_from_lobj($subpart->Lohko->Linkki, $subpart->sid);
    $variables['navi'] = _add_navi($subpart->Navigointi, $subpart->sid);
  }
  else {
    $variables['hit_info'] = 'No results';
    $variables['no_results'] = 'true';
  }
  // Provide alternate search results template.
  $variables['template_files'][] = 'libdbsearch-partworks-' . $variables['type'];
}

/**
 * Shows work's sub object related information on page.
 *
 * @param <type> $variables work information
 * @return void return if no data to show
 */
function template_preprocess_libdbsearch_partwork(&$variables) {
  $result = $variables['result'];
  $variables['so_number'] = $result->Sarake1->Teksti;
  $variables['so_author'] = _make_var($result->Sarake2, $result->sid); //$result->Sarake[1]->Teksti;
  $variables['so_name'] = _make_var($result->Sarake3, $result->sid);
  // Provide alternate search result template.
  $variables['template_files'][] = 'libdbsearch-partwork-' . $variables['type'];
}

/**
 * Creates navigation between result pages.
 *
 * @param object $navi XML data from navaigation
 * @return string navigation page data
 */
function _add_navi($navi, $sid) {
  $ret = '';
  if (isset($navi)) {
    $origo = $navi;
    $ret = libdb_t($origo->Teksti);
    foreach ($origo->Valinta as $result) {
      if (isset($result->Haettu)) {
        $ret .= libdb_t(trim($result->Haettu));
      }
      elseif (isset($result->Linkki)) {
        $ret .= _link_from_lobj($result->Linkki, $sid);
      }
      else {
        $ret .= trim(libdb_t((string) $result));
      }
      $ret .= ' ';
    }
  }
  return $ret;
}

/**
 * Creates rows if needed.
 *
 * @param string text data
 * @return string text in rows
 */
function _get_value($str) {
  if (is_array($str)) {
    $ret = '';
    foreach ($str as $val) {
      $ret .= $val . ' ';
    }
  }
  else {
    $ret = $str;
  }
  return $ret;
}

/**
 * Creates link address based to Drupal system
 * from XML link object.
 *
 * @param string $osoite link object
 * @param string $sid key to search db
 * @return string proper address for Drupal link
 */
function _link_from_lobj($linkki, $sid = NULL) {
  return _link_from_text($linkki->Losoite, libdb_t((string)$linkki->Lteksti), $sid);
}

/**
 * Examines given value and if it is link marked,
 * creeates a link, else returns text.
 *
 * @param string $text marking to check
 * @param string $sid key to search db
 * @return string result marking
 */
function _add_link_or_text($text, $sid) {
  if (isset($text->Linkki)) {
     return _link_from_lobj($text->Linkki, $sid);
  }
  else {
    return (string) $text;
  }
}

/**
 * Creates link address based to Drupal system.
 *
 * @param string $losoite actual address
 * @param string $lteksti text to show
 * @param string $sid key to search db
 * @return string proper address for Drupal link
 */
function _link_from_text($losoite, $lteksti, $sid=NULL) {
  if ($sid !== NULL && !empty($sid)) {
    $palat = explode('.asp?', $losoite);
    if (count($palat) == 2) {
      $osoite = $palat[0] . '/' . $sid . '&' . $palat[1];
    }
    else {
      $osoite .= '&sid=' . $sid;
    }
  }
  else {
    $osoite = str_replace(".asp?", "/", $losoite);
  }

  $show = libdb_t($lteksti);
  return '<a href="' . url('libdbsearch/') . $osoite . '">' . $show . '</a>';
}

/**
 * Translate's texts came from library database system.
 *
 * @param string $teksti text to translate
 * @return string translated text
 */
function libdb_t($teksti) {
  $lista = array();
  include('includes/extractiontable.php');
  $my_teksti = str_replace(
      array("ä", "Ä", "ö", "Ö", "å", "Å", " "), 
      array("a", "A", "o", "O", "o", "O", "_"), $teksti);
  if (empty($lista[$my_teksti])) {
    return $teksti;
  }
  else {
    return t($lista[$my_teksti]);
  }
}

/**
 * Creates work info to shown in page.
 *
 * @param object $data work XML data
 * @param string $sid key to search db
 * @return string work information row data
 */
function _make_var($data, $sid = NULL) {
  if (empty($data)) {
    return FALSE;
  }
  $mem = '';
  if (isset($data->Teksti)) {
    $mem = extract_teksti($data->Teksti);
  }
  else { // Link
    foreach ($data as $link) {
      if (!empty($mem)) {
        $mem .= '<br />';
      }
      $mem .= _link_from_lobj($link->Linkki, $sid);
      if (isset($link->Linkki->Lextrateksti)) {
        $mem .= ' ' . $link->Linkki->Lextrateksti;
      }
    }
  }
  return $mem;
}

/**
 * Split text to rows.
 *
 * @param string $teksti text to split to rows
 * @return string text in multirow format
 */
function extract_teksti($teksti) {
  $mem = '';
  foreach ($teksti as $text) {
    if (!empty($mem)) {
      $mem .= '<br />';
    }
    $mem .= $text;
  }
  return $mem;
}

/**
 * Create prepopulated link to content type "suosittelut"
 * if it is in system and also prepopulate-module must be
 * in use.
 *
 * @param type $teos
 * @param type $clink
 * @return string 
 */
function libdbsearch_node_add_link($teos, $clink = '') {
  $ret = '';
  $video = array('Blu-ray', 'AV-aineisto', 'DVD-levy', 'Videokasetti');
  $recording = array('Kielikurssi', 'CD-levy', 'Äänilevy', 'Kasetti', 'Äänite');
  $audiobook = array('Äänikirja');
  $game = array('CD-ROM-levy');
  
  if (module_exists('prepopulate') === FALSE) {
    return $ret;
  }
  $n = node_get_types();
  if (!isset($n['suosittelut'])) {
    return $ret;
  }
  if (user_access('create suosittelut content') == FALSE ) {
    return $ret;
  }
  for ($i = 0; $i < count($teos->Rivi); $i++) {
    if ($teos->Rivi[$i]->Selite == 'Aineistolaji') {
      $alaji = libdbsearch_get_str($teos->Rivi[$i]);
    }
    elseif ($teos->Rivi[$i]->Selite == 'Tekijä / esittäjä') {
      $tekija = libdbsearch_get_str($teos->Rivi[$i]);
    }
    elseif ($teos->Rivi[$i]->Selite == 'Teoksen nimi') {
      $tnimi = libdbsearch_get_str($teos->Rivi[$i]);
    }
    elseif ($teos->Rivi[$i]->Selite == 'Sarja') {
      $sarja = libdbsearch_get_str($teos->Rivi[$i]);
    }
    elseif ($teos->Rivi[$i]->Selite == 'ISBN') {
      $isbn = libdbsearch_get_str($teos->Rivi[$i]);
    }
    elseif ($teos->Rivi[$i]->Selite == 'Julkaisutiedot') {
      $raw_julk = libdbsearch_get_str($teos->Rivi[$i]);
      $julk = explode(',', $raw_julk);
      if (count($julk) == 2) {
        $kustantaja = $julk[0];
        $matches = NULL;
        preg_match_all("/[0-9]{4}/i", $julk[1], $matches);
        $vuosi = $matches[0][0];
      }
    }
  }

  $params = '';
  if (!empty($tekija)) {
    $params .= 'edit[field_tekija][0][value]=' . $tekija;
  }
  if (!empty($tnimi)) {
    if (!empty($params)) {
      $params .= '&';
    }
    $params .= 'edit[field_teoksen_nimi][0][value]=' . $tnimi;
    $params .= '&edit[title]=' . $tnimi;
  }
  if (!empty($sarja)) {
    if (!empty($params)) {
      $params .= '&';
    }
    $params .= 'edit[field_sarja][0][value]=' . $sarja;
  }
  if (!empty($vuosi)) {
    if (!empty($params)) {
      $params .= '&';
    }
    $params .= 'edit[field_julkaisuvuosi][0][value][date]=' . $vuosi;
  }
  if (!empty($kustantaja)) {
    if (!empty($params)) {
      $params .= '&';
    }
    $params .= 'edit[field_kustantaja][0][value]=' . $kustantaja;
  }
  if (!empty($isbn)) {
    if (!empty($params)) {
      $params .= '&';
    }
    $params .= 'edit[field_isbn][0][value]=' . $isbn;
  }
  if (!empty($clink)) {
    if (!empty($params)) {
      $params .= '&';
    }
    //$arvo = $_SERVER['HTTP_HOST'] . url('libdbsearch/') . $clink;
    $arvo = 'libdbsearch/' . $clink;
    $params .= 'edit[field_saatavuus][0][url]=' . $arvo;
  }
  
  if (!empty($alaji)) {
    if (!empty($params)) {
      $params .= '&';
    }
    if (in_array($alaji, $video)) {
      $alaji = 'Video';
    }
    elseif (in_array($alaji, $recording)) {
      $alaji = 'Recording';
    }
    elseif (in_array($alaji, $audiobook)) {
      $alaji = 'Audiobook';
    }
    elseif (in_array($alaji, $game)) {
      $alaji = 'Game';
    } 
    else {
      $alaji = 'Book';
    }
    $params .= 'edit[field_aineistolaji][value]=' . $alaji;
  }

  $show = t('Create a new tip');
  $ret = '<a href="' . url('node/add/suosittelut') . '?' . $params . '">' . $show . '</a>';

  return $ret;
}

function libdbsearch_get_str($row) {
  if (isset($row->Tieto->Teksti)) {
    $str = (string) $row->Tieto->Teksti;
  }
  else { // Link
    $str = (string) $row->Tieto->Linkki->Lteksti;
  }
  return $str;
}
