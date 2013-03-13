<?php

/**
 * @file
 * Web-Origo dropdown list readers.
 *
 * Last modified: 13.03.2013 Mika Hatakka
 */

/**
 * 
 */
class LibdbUpdate {

  /**
   * Check if list database and Origo lists are equal. Lists are used
   * to keep library, class, language, marial and departments info.
   */
  public function origoUpdateLists() {
    $origo = new LibdbConnection();
    $origo->init('tarkkahaku', '');
    $page = $origo->askPage();

    if ($page !== NULL) {
      $ret = $this->origoParseSelectContent($page);
      if ($ret !== NULL) {
        $ret1 = libdbsearch_read_list(1);
        if ((array_diff($ret['piste'], $ret1) == array_diff($ret1, $ret['piste'])) !== TRUE) {
          $this->listToDb($ret['piste'], 1);
        }
        $ret2 = libdbsearch_read_list(2);
        if ((array_diff($ret['luokkaryhma'], $ret2) == array_diff($ret2, $ret['luokkaryhma'])) !== TRUE) {
          $this->listToDb($ret['luokkaryhma'], 2);
        }
        $ret3 = libdbsearch_read_list(3);
        if ((array_diff($ret['kieliryhma'], $ret3) == array_diff($ret3, $ret['kieliryhma'])) !== TRUE) {
          $this->listToDb($ret['kieliryhma'], 3);
        }
        $ret4 = libdbsearch_read_list(4);
        if ((array_diff($ret['materiaaliryhma'], $ret4) == array_diff($ret4, $ret['materiaaliryhma'])) !== TRUE) {
          $this->listToDb($ret['materiaaliryhma'], 4);
        }
        $ret5 = libdbsearch_read_list(5);
        if ((array_diff($ret['osastoryhma'], $ret5) == array_diff($ret5, $ret['osastoryhma'])) !== TRUE) {
          $this->listToDb($ret['osastoryhma'], 5);
        }
        return TRUE;
      }
    }
    watchdog(WATCHDOG_WARNING, 'Origo lists update failed, no information from Origo.');
    return FALSE;
  }

  /**
   * Get the dropdown list options from all dropdowns in the page.
   *
   * @param string $page web-origo response page
   * @return array array of dropdpwnlist contents
   */
  private function origoParseSelectContent($page) {
    $doc = new DOMDocument();
    @$doc->loadHTML($page);
    $elements = $doc->getElementsByTagName('select');
    $ret = NULL;
    if (!is_null($elements) && $elements->length > 0) {
      foreach ($elements as $element) {
        if ($element->hasAttribute('name')) {
          $ret[$element->attributes->getNamedItem('name')->nodeValue] =
              $this->origoGetOptions($element->childNodes);
        }
        else {
          $ret[] = $this->origoGetOptions($element->childNodes);
        }
      }
    }
    return $ret;
  }

  /**
   * Returns dropdown list options.
   *
   * @param DOMNodeList $nodeList
   * @return array value name pairs of options
   */
  private function origoGetOptions($nodeList) {
    $ret = NULL;
    foreach ($nodeList as $node) {
      if ($node->hasAttribute('value')) {
        $ret[$node->attributes->getNamedItem('value')->nodeValue] = $node->nodeValue;
      }
      else {
        $ret[] = $node->nodeValue;
      }
    }
    return $ret;
  }

  /**
   * Write list content to the list database.
   *
   * @param <type> $db list
   * @param <type> $ind type of list
   */
  private function listToDb($db, $ind) {
    db_delete('libdbsearch_lists')
        ->condition('tyyppi', $ind)
        ->execute();

    $i = 0;
    foreach ($db as $k => $v) {
      $i++;
      $values[] = array(
        'nro' => $i,
        'tyyppi' => $ind,
        'arvo' => $k,
        'teksti' => $v,
      );
    }
    $insert = db_insert('libdbsearch_lists')->fields(array('nro', 'tyyppi', 'arvo', 'teksti'));
    foreach ($values as $value) {
      $insert->values($value);
    }
    $insert->execute();
  }

}