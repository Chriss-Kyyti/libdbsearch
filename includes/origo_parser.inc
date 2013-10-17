<?php

/**
 * @file
 * Class for sending a search to Origo library database and
 * extracting information from erceived web pages.
 * Result is in XML form.
 *
 * Last modified: 17.10.2013 Mika Hatakka
 */

/**
 * Parse Origo library database sysytem data from HTML page.
 */
class OrigoParser {

  private $palvelin;
  private $type;
  private $page;

  const HAKU = 'haku';
  const SELAUS = 'selauslista';
  const TEOS = 'teos';
  const OSAKOHDE = 'osakohteet';

  /**
   * Class constructor, set needed parameters
   *
   * @param string $palvelin server where database is located
   * @param string $type search type
   * @param string $hakuehdot search keywords
   */
  public function __construct($page, $type, $server = '') {
    $this->page = $page;
    $this->type = $type;
    $this->palvelin = $server;
  }

  /**
   * Create XML formed data from the received search result.
   *
   * @return array search result data in XML
   */
  public function constructXML() {
    if ($this->page == 'Internal Server Error') {
      $level1xml = $this->tulosta_virhe_ilmoitus('Internal Server Error');
      return $this->luo_tagi('Libdbsearch', $level1xml);
    }
    if (strpos($this->page, 'Teoksia ei löytynyt yhtään kappaletta.') !== FALSE) {
      $level1xml = $this->tulosta_virhe_ilmoitus('No results');
      return $this->luo_tagi('Libdbsearch', $level1xml);
    }
    switch ($this->type) {
      case self::HAKU :
        $tulosXML = $this->pura_aineistoittain();
        if (strpos($tulosXML, '<Virhe>TRUE') !== FALSE) {
          $tulosXML = $this->pura_selauslista();
        }
        break;
      case self::SELAUS :
        $tulosXML = $this->pura_selauslista();
        break;
      case self::TEOS :
        $tulosXML = $this->pura_teos();
        break;
      case self::OSAKOHDE :
        $tulosXML = $this->pura_osakohde();
        break;
      default:
        $info_xml = $this->tulosta_virhe_ilmoitus('Unknown type.');
        $tulosXML = '<Message>' . $info_xml . '</Message>';
        break;
    }
    return $tulosXML;
  }

//    public function get_form_lists() {
//        $this->do_search();
//        return $this->hae_oheistieto($this->page);
//    }
  //-------------------------------------------------------
  // Private methods

  /**
   * Extracts all needed data from origo webpage 'aineistoittain.asp'
   *
   * @return string result XML data
   */
  private function pura_aineistoittain() {
    $homepage = $this->page;
    $otsikko = $this->etsi_otsikko($homepage, 3);
    if ($otsikko !== FALSE) {
      $level1xml = $this->luo_tagi('Otsikko', $otsikko);
      $level2xml = $this->luo_tagi('Teksti', $this->teoksia_loytynyt($homepage));
      $level1xml .= $this->luo_tagi('Hakuinfo', $level2xml);
      $linkit = $this->pura_linkit($homepage);
      $taulukko = $this->pura_taulukko($homepage);
      $totsikot = $this->pura_taulukon_otsikot($taulukko[0][0]);
      $tsarakkeet = $this->pura_sarakkeet($taulukko[0][0]);
      $level3xml = '';
      for ($i = 0; $i < count($tsarakkeet[0]); $i += count($totsikot[0])) {
        // We do not know the exact order, so must be cleared every round.
        $level4xml = '';
        foreach ($totsikot[0] as $key => $val) {
          if (stripos($val, 'laji') !== FALSE) {
            $apustr = $this->pura_linkit($tsarakkeet[0][$key + $i]);
            if ($apustr === FALSE) {
              $level4xml .= $this->luo_tagi('Laji', $tsarakkeet[1][$key + $i]);
            } else {
              $level5xml = $this->luo_linkki($apustr[1][0], $apustr[2][0]);
              $level4xml .= $this->luo_tagi('Laji', $level5xml);
            }
          }
          if (stripos($val, 'Löyty') !== FALSE) {
            $level4xml .= $this->luo_tagi('Maara', $tsarakkeet[1][$key + $i]);
          }
        }
        $level3xml .= $this->luo_tagi('Aineistolaji', $level4xml);
      }
      $level2xml = $this->luo_tagi('Aineistolajittain', $level3xml);
      $level1xml .= $this->luo_tagi('Hakutulos', $level2xml);
      $level2xml = $this->luo_linkki_listalta($linkit, 'naytakaikki=1', 1, FALSE);
      if ($level2xml != '') {
        $level1xml .= $this->luo_tagi('Lohko', $level2xml);
      }
      $level0xml = $this->luo_tagi('Aineistolajittain_tulos', $level1xml);
    } else {
      $level1xml = $this->tulosta_virhe_ilmoitus('Virhe suoritettaessa hakua');
      $level0xml = $this->luo_tagi('Aineistolajittain_tulos', $level1xml);
    }
    return $level0xml;
  }

  /**
   * Extracts all needed data from origo webpage 'selauslista.asp'
   *
   * @return string result XML data
   */
  private function pura_selauslista() {
    $homepage = $this->page;
    $teostasivulla = 20;
    if (!empty($homepage)) {
      $otsikko = $this->etsi_otsikko($homepage, 1);
      if ($otsikko !== FALSE) {
        $level1xml = $this->luo_tagi('Otsikko', $otsikko);
        $tables = $this->pura_taulukko($homepage);
        if (count($tables[0]) >= 2) {
          // First the information if we have any results
          $colums = $this->pura_sarakkeet($tables[0][0]);
          $curPage = 0;
          if ($this->pura_kohde($colums[0][0], "/Teoksia löytyi yhteensä [0-9]+ kappaletta/") !== FALSE) {
            // Page number information
            if (($allNum = $this->pura_kohde($colums[0][0], "/[0-9]+/")) !== FALSE) {
              if ((count($allNum[0]) == 3) && ($teostasivulla > 0)) {
                // Several pages, lets count current page
                $curPage = ($allNum[0][1] + $teostasivulla - 1) / $teostasivulla;
                $maxPage = (int) (($allNum[0][0] - 1) / $teostasivulla) + 1;
              } elseif ((count($allNum[0]) == 4) && ($teostasivulla > 0)) {
                // Add to handle case where more than 1000 results.
                $curPage = ($allNum[0][2] + $teostasivulla - 1) / $teostasivulla;
                $maxPage = (int) (($allNum[0][0] - 1) / $teostasivulla) + 1;
              }
            } else {
              $curPage = -1;
            }
          } else {
            $curPage = -1;
          }
          if (strlen(strip_tags($colums[1][0])) > 0) {
            $tekstit = preg_split("/(<br \/>)|(<br>)|(<br\/>)/", $colums[1][0]);
            $level2xml = '';
            foreach ($tekstit as $t) {
              $level2xml .= $this->luo_tagi('Teksti', $t);
            }
            $level1xml .= $this->luo_tagi('Hakuinfo', $level2xml);
          }
          $linkit = $this->pura_linkit($colums[0][1]);
          $level2xml = $this->luo_linkki_listalta($linkit, 'aineistoittain.asp', 1, FALSE);
          $level1xml .= $this->luo_tagi('Lohko', $level2xml);
          $tekstit = preg_split("/(<br \/>)|(<br>)|(<br\/>)/", $colums[0][1]);
          if (($curPage != -1) && ($maxPage > 1)) {
            $level2xml = $this->luo_tagi('Teksti', 'Sivu:&nbsp;', array('ncr' => 'TRUE'));
            $level2xml .= $this->tee_navigointi($curPage, $maxPage, $tekstit[1]);
            $level1xml .= $this->luo_tagi('Navigointi', $level2xml);
          }
          // The result table
          $colums = $this->pura_sarakkeet($tables[0][1]);
          $header = $this->pura_taulukon_otsikot($tables[0][1]);
          $totsikot = array('', 'Tekijä', 'Teoksen nimi', 'Aineistolaji', 'Vuosi', 'Luokka');
          $res = TRUE;
          for ($i = 1; (($i < 6) && ($i < count($header[0]))); $i++) {
            if (strip_tags($header[0][$i]) == $totsikot[$i]) {
              continue;
            }
            $res = FALSE;
          }
          if ($res == TRUE) {
            $level3xml = '';
            for ($i = 0; $i < count($colums[1]); $i += 6) {
              $level4xml = '';
              $level4xml .= $this->luo_tagi('Tekija', $this->tarkista_haettu($colums[1][$i + 1]));
              $apustr = $this->pura_linkit($colums[1][$i + 2]);
              $level5xml = $this->luo_linkki($apustr[1][0], $apustr[2][0]);
              $level4xml .= $this->luo_tagi('Teoksen_nimi', $level5xml);
              $level4xml .= $this->luo_tagi('Teoksen_aineistolaji', $this->tarkista_haettu($colums[1][$i + 3]));
              $level4xml .= $this->luo_tagi('Julkaisuvuosi', $this->tarkista_haettu($colums[1][$i + 4]));
              $level4xml .= $this->luo_tagi('Luokka', $this->tarkista_haettu($colums[1][$i + 5]));
              $level3xml .= $this->luo_tagi('Teos', $level4xml);
            }
            $level2xml = $this->luo_tagi('Selausotsikot', $this->luo_selaus_otsikot($header[0], 2));
            $level2xml .= $this->luo_tagi('Selauslista', $level3xml);
            $level1xml .= $this->luo_tagi('Hakutulos', $level2xml);
            if (count($tables[0]) == 3) {
              // The footer table
              $colums = $this->pura_sarakkeet($tables[0][2]);
              $level2xml = $this->luo_tagi('Teksti', $colums[1][0], array('ncr' => 'TRUE'));
              $level1xml .= $this->luo_tagi('Lohko', $level2xml);
              if (($curPage != -1) && ($maxPage > 1)) {
                $level2xml = $this->luo_tagi('Teksti', 'Sivu:&nbsp;', array('ncr' => 'TRUE'));
                $level2xml .= $this->tee_navigointi($curPage, $maxPage, $colums[1][1]);
                $level1xml .= $this->luo_tagi('Navigointi', $level2xml);
              }
            }
          } else {
            $level1xml = $this->tulosta_virhe_ilmoitus('Otsikkotiedot eivät täsmää!');
          }
        } else {
          $level1xml = $this->tulosta_virhe_ilmoitus('Ei kolmea taulukkoa!');
        }
      } else {
        $level1xml = $this->tulosta_virhe_ilmoitus();
      }
    } else {
      $level1xml = $this->tulosta_virhe_ilmoitus(' Ei vastausta!');
    }
    $level0xml = $this->luo_tagi('Selauslista_tulos', $level1xml);
    return $level0xml;
  }

  /**
   * Extracts all needed data from origo webpage 'teos.asp'
   *
   * @return string result XML data
   */
  function pura_teos() {
    $homepage = $this->page;
    $otsikko = $this->etsi_otsikko($homepage, 1);
    if ($otsikko !== FALSE) {
      $level1xml = $this->luo_tagi('Otsikko', $otsikko);
      $linkit = $this->pura_linkit($homepage);
      $level1xml .= $this->luo_selailu_navigointi($linkit);
      // Work information is a table inside a table,
      // that's why so complicated process
      $tp = $this->maarita_rajat($homepage);
      if ($tp === FALSE) {
        ; /* Broken */
      }
      if ($tp[1]['alku'] < $tp[0]['loppu']) { // Inner table
//                $xmlmem = '';
        $tulos = substr($homepage, $tp[1]['alku'], $tp[1]['loppu'] - $tp[1]['alku'] + 8);
        if (($colums = $this->pura_sarakkeet($tulos)) !== FALSE) {
          $level2xml = $this->muodosta_teos_tiedot($colums);
          $level1xml .= $this->luo_tagi('Teostiedot', $level2xml);
        }
        if (isset($tp[2])) {
          if ($tp[2]['loppu'] < $tp[0]['loppu']) { // Second inner table
            $level1xml .= $this->extract_inner2($tp, $homepage);
          }
        }
      } // End of Inner table(s)
      // Check picture
      $pics = $this->pura_kuvat($homepage);
      if (!empty($pics)) {
        foreach ($pics as $pic) {
          $level1xml .= $this->luo_tagi('Kuva', $pic);
        }
      }
      $osa_page = explode('>Lukijoiden arvostelut teoksesta<', $homepage);
      $osa_sivu = explode('>Saatavuustiedot<', $osa_page[0]);
      if (!empty($osa_sivu[1])) {
        $level1xml .= $this->extract_availability($osa_sivu[1], 'Saatavuustiedot');
      } else {
        $osa_sivu = explode('>Saatavuustiedot muissa kirjastoissa<', $osa_page[0]);
        if (!empty($osa_sivu[1])) {
          $level1xml .= $this->extract_availability($osa_sivu[1], 'Saatavuustiedot muissa kirjastoissa');
        }
      }
      $osa_sivu = explode('kirjasto</h', $osa_page[0]);
      if (!empty($osa_sivu[1])) {
        $otsikko = $this->etsi_otsikko($osa_sivu[0] . 'kirjasto</h4>', 4);
        $level1xml .= $this->extract_chosen_availability($osa_sivu[1], $otsikko);
      }
      $level2xml = $this->luo_linkki_listalta($linkit, 'Varaa teos', 2, TRUE);
      if (!empty($level2xml)) {
        $level1xml .= $this->luo_tagi('Lohko', $level2xml);
      }
      $level2xml = $this->luo_linkki_listalta($linkit, 'Tarkat tiedot', 2, FALSE);
      if (!empty($level2xml)) {
        $level1xml .= $this->luo_tagi('Lohko', $level2xml);
      }
      $level2xml = $this->luo_linkki_listalta($linkit, 'Lainaa sähköinen kirja', 2, TRUE);
      if (!empty($level2xml)) {
        $level1xml .= $this->luo_tagi('Lohko', $level2xml);
      }
      $level2xml = $this->luo_linkki_listalta($linkit, 'Näytä sisältö / kappaleet', 2, FALSE);
      if (!empty($level2xml)) {
        $level1xml .= $this->luo_tagi('Lohko', $level2xml);
      }
      $level2xml = $this->luo_linkki_listalta($linkit, 'Esittelyteksti', 2, TRUE);
      if (!empty($level2xml)) {
        $level1xml .= $this->luo_tagi('Lohko', $level2xml);
      }
      $level2xml = $this->luo_linkki_listalta($linkit, 'Saatavuustiedot', 2, FALSE);
      if (!empty($level2xml)) {
        $level1xml .= $this->luo_tagi('Lohko', $level2xml);
      }
    } else {
      $level1xml = $this->tulosta_virhe_ilmoitus();
    }
    $level1xml .= $this->luo_selailu_navigointi($linkit);
    $level0xml = $this->luo_tagi('Teoksen_tiedot', $level1xml);
    return $level0xml;
  }

  /**
   * Extracts works contents / songs.
   *
   * @return string result XML data
   */
  function pura_osakohde() {
    $homepage = $this->page;
    $otsikko = $this->etsi_otsikko($homepage, 2);
    if ($otsikko !== FALSE) {
      $header_link = $this->etsi_otsikko_linkki($homepage, 2);
      if ($header_link !== FALSE) {
        $osoite = $this->tarkasta_amp($header_link[1][0]);
        $otsikko = $this->luo_linkki($osoite, $header_link[2][0]);
      }
      $level1xml = $this->luo_tagi('Otsikko', $otsikko);

      $is_navi = TRUE;
      $kplmaara = $this->pura_kohde($homepage, "/([0-9]+ kappaletta)/");
      if ($kplmaara !== FALSE) {
        // Page number information
        if (($allNum = $this->pura_kohde($kplmaara[0][0], "/[0-9]+/")) !== FALSE) {
          $full_amount = $allNum[0][0];
          $level1xml .= $this->luo_tagi('Teksti', 'Sisältö (' . $full_amount . ' kappaletta)');
        } else {
          $is_navi = FALSE;
        }
      } else {
        $is_navi = FALSE;
      }
      $tables = $this->pura_taulukko($homepage);
      $header = $this->pura_taulukon_otsikot($tables[0][0]);
      if (count($header[0]) != 3) {/* Virhe */
      }
      $level2xml = $this->luo_tagi('Totsikot', $this->luo_selaus_otsikot($header[0]));
      $my_rows = $this->pura_rivit($tables[0][0]);
      for ($i = 1; $i < count($my_rows[0]); $i++) {
        $solut = explode("<td>", $my_rows[0][$i]);
        $sana = explode("</td>", $solut[1]);
        $level3xml = $this->luo_tagi('Sarake1', $this->luo_tagi('Teksti', $sana[0]));
        $sana = explode("</td>", $solut[2]);
        $level3xml .= $this->luo_tagi('Sarake2', $this->add_link_or_text($sana[0]));
        $sana = explode("</td>", $solut[3]);
        $level3xml .= $this->luo_tagi('Sarake3', $this->add_link_or_text($sana[0]));
        $level2xml .= $this->luo_tagi('Rivi', $level3xml);
      }
      $level1xml .= $this->luo_tagi('Osakohteet', $level2xml);
    }
    if ($is_navi === TRUE) {
      // Navigation
      $hanta = explode('</table>', $homepage);
      if (count($hanta) > 1) {
        $apu = $this->pura_kohde($hanta[1], "/Kappaleet (.+),/");
        $level1xml .= $this->luo_tagi('Teksti', substr($apu[0][0], 0, -1));
        $tekstit = preg_split("/(<br \/>)|(<br>)|(<br\/>)/", $hanta[1]);
        $apu = explode(',', $tekstit[1]);
        if (isset($apu[1])) {
          $navit = explode('&nbsp;', $apu[1]);
        } else {
          $navit = explode('&nbsp;', $apu[0]);
        }
        $level2xml = '';
        foreach ($navit as $val) {
          if (empty($val)) {
            continue;
          }
          $tulos = $this->add_link_or_text($val);
          if (strpos($tulos, '<Teksti>') !== FALSE) {
            if ((strpos($tulos, 'Seuraava') !== FALSE) ||
                    (strpos($tulos, 'Edellinen') !== FALSE)) {
              $level2xml .= str_replace('Teksti', 'Valinta', $tulos);
              continue;
            }
            $tulos = str_replace('Teksti', 'Haettu', $tulos);
          }
          $level2xml .= $this->luo_tagi('Valinta', $tulos);
        }
        $level1xml .= $this->luo_tagi('Navigointi', $level2xml);
        // Back to work information
        $alink = $this->pura_linkit($tekstit[3]);
        $osoite = $this->tarkasta_amp($alink[1][0]);
        $level2xml = $this->luo_linkki($osoite, $alink[2][0]);
        $level1xml .= $this->luo_tagi('Lohko', $level2xml);
      }
    }
    $level0xml = $this->luo_tagi('Osakohteen_tiedot', $level1xml);
    return $level0xml;
  }

  //-------------------------------------------------------
  // Common methods for page handling.

  /**
   * To find html header marking from given string.
   *
   * @param string $jono string where to find
   * @param integer $koko header size, 1 - 6
   * @return mixed return header text, without html markings or
   *               FALSE if not found
   */
  private function etsi_otsikko($jono, $koko) {
    if ($koko < 1 || $koko > 6) {
      return FALSE;
    }
    $etsitaan = "/<h$koko>.+<\/h$koko>/";
    $riveja = preg_match_all($etsitaan, $jono, $tulos);
    if ($riveja == 1) {
      return strip_tags($tulos[0][0]);
    }
    return FALSE;
  }

  /**
   * Check if header is also a link.
   *
   * @param string $jono string where to find
   * @param integer $koko header size, 1 - 6
   * @return mixed return link if found else FALSE if not found
   */
  private function etsi_otsikko_linkki($jono, $koko) {
    if ($koko < 1 || $koko > 6) {
      return FALSE;
    }
    $etsitaan = "/<h$koko>.+<\/h$koko>/";
    $riveja = preg_match_all($etsitaan, $jono, $tulos);
    if ($riveja == 1) {
      return $this->pura_linkit($tulos[0][0]);
    }
  }

  /**
   * Create's XML-tag from given vallues.
   *
   * @param string $nimi tag name
   * @param string $tieto tag value
   * @param string $param tag attribute
   * @return string the XML tag-string
   */
  private function luo_tagi($nimi, $tieto, $param = null) {
    $lisa = '';
    $taki = explode('/', $nimi);
    $taki[0] = str_replace(" ", "_", trim($taki[0]));
    if ($param != null) {
      foreach ($param as $key => $val) {
        $lisa .= ' ' . $key . '="' . $val . '"';
      }
      $lisa .= ' ';
    }
    if ($tieto == '') {
      $paluu = '<' . $taki[0] . $lisa . ' />';
    } else {
      $paluu = '<' . $taki[0] . $lisa . '>';
      $paluu .= trim($tieto);
      $paluu .= '</' . $taki[0] . '>';
    }
    return $paluu;
  }

  /**
   * Add s new link from parameters.
   *
   * @param string $osoite link address
   * @param string $teksti link text
   * @param boolean $uuteen open to a new window
   * @return string result XML data
   */
  private function luo_linkki($osoite, $teksti = NULL, $extratext = NULL, $uuteen = FALSE) {
    $osoite = str_replace("'", '', $osoite);
    $osoite = str_replace('"', '', $osoite);
    if ($teksti == NULL) {
      $teksti = $osoite;
    } else {
      $teksti = $this->tarkista_haettu($teksti);
    }
    $taso2xml = $this->luo_tagi('Losoite', $osoite);
    $taso2xml .= $this->luo_tagi('Lteksti', $teksti);
    if ($uuteen == TRUE) {
      $taso2xml .= $this->luo_tagi('Lnewwnd', 'TRUE');
    }
    if ($extratext != NULL) {
      $taso2xml .= $this->luo_tagi('Lextrateksti', $extratext);
    }
    $taso1xml = $this->luo_tagi('Linkki', $taso2xml);
    return $taso1xml;
  }

  /**
   * Does search of $etsitaan in $kohde,
   * results are returned array.
   *
   * More information on result array, see:
   * http://fi.php.net/manual/en/function.preg-match-all.php
   *
   * @param string $kohde where to search
   * @param string $etsitaan what to search
   * @return mixed result array or FALSE if fails
   */
  private function pura_kohde($kohde, $etsitaan) {
    $riveja = preg_match_all($etsitaan, $kohde, $tulos);
    if ($riveja == 0) {
      return FALSE;
    } else {
      return $tulos;
    }
  }

  /**
   * Search all links from given source string.
   *
   * @param string $kohde where to search
   * @return mixed result array or FALSE if fails
   */
  private function pura_linkit($kohde) {
    // Find links from cell contents
    // [0] original
    // [1] address
    // [2] link text
    return $this->pura_kohde($kohde, "/<a href=(.+)>(.+)<\/a>/siU");
  }

  /**
   * Search all table from given source string.
   *
   * @param string $kohde where to search
   * @return mixed result array or FALSE if fails
   */
  private function pura_taulukko($kohde) {
    return $this->pura_kohde($kohde, "/<table.*>(.+)<\/table>/siU");
  }

  /**
   * Search all table hesders from given source string.
   *
   * @param string $kohde where to search
   * @return mixed result array or FALSE if fails
   */
  private function pura_taulukon_otsikot($kohde) {
    return $this->pura_kohde($kohde, "/<th.+<\/th>/siU");
  }

  /**
   * Search all table colums from given source string.
   *
   * @param string $kohde where to search
   * @return mixed result array or FALSE if fails
   */
  private function pura_rivit($kohde) {
    return $this->pura_kohde($kohde, "/<tr.*>(.+)<\/tr>/siU");
  }

  /**
   * Search all table colums from given source string.
   *
   * @param string $kohde where to search
   * @return mixed result array or FALSE if fails
   */
  private function pura_sarakkeet($kohde) {
    return $this->pura_kohde($kohde, "/<td.*>(.+)<\/td>/siU");
  }

  /**
   * Search information on found works.
   *
   * @param string $homepage where to find
   * @return string result or FLASE if fails
   */
  private function teoksia_loytynyt($homepage) {
    // Found works
    $etsitaan = "/Teoksia löytyi yhteensä [0-9]+ kappaletta/";
    $riveja = preg_match_all($etsitaan, $homepage, $tulos);
    if ($riveja == 1) {
      // Number of found works
      return $tulos[0][0];
    } else {
      $etsitaan = "/Teoksia ei löytynyt yhtään kappaletta/";
      $riveja = preg_match_all($etsitaan, $homepage, $tulos);
      if ($riveja == 1) {
        // Nothing found
        return $tulos[0][0];
      }
    }
    return FALSE;
  }

  /**
   * Add error message result XML data.
   *
   * @param string $virhetxt error message to add, if given
   * @return string result string
   */
  private function tulosta_virhe_ilmoitus($virhetxt = null) {
    $taso1xml = $this->luo_tagi('Otsikko', 'Haku');
    $taso1xml .= $this->luo_tagi('Teksti', 'Virhe suoritettaessa hakua, ', array('ncr' => 'TRUE'));
    if (!empty($virhetxt)) {
      $taso1xml .= $this->luo_tagi('Teksti', $virhetxt);
    }
    $taso1xml .= $this->luo_linkki('sanahaku.php', 'Palaa sanahakuun');
    $taso1xml .= $this->luo_tagi('Virhe', 'TRUE');
    return $taso1xml;
  }

  /**
   * Find wanted link from link lista and
   * create XML presentation from that link.
   *
   * @param string $linkit link list
   * @param string $ehto criteria to found
   * @param string $etsitaan weather searching from link or text
   * @param string $newwnd open in new window
   * @return string result XML string
   */
  private function luo_linkki_listalta($linkit, $ehto, $etsitaan = 1, $newwnd = FALSE) {
    $xml = '';
    if (!empty($linkit)) {
      foreach ($linkit[$etsitaan] as $key => $val) {
        if (stripos($val, $ehto) !== FALSE) {
          $uusi = $this->tarkasta_amp($linkit[1][$key]);
          if($ehto == 'Varaa teos' && !empty($this->palvelin)) {
            $uusi = $this->palvelin . 'index.asp?url=' . $uusi;
          }
          $xml = $this->luo_linkki($uusi, $linkit[2][$key], NULL, $newwnd);
          break;
        }
      }
    }
    return $xml;
  }

  /**
   * Check that all '&' are changed to '&amp;'
   *
   * @param string $linkki link to check
   * @return string proper link
   */
  private function tarkasta_amp($linkki) {
    if (strpos($linkki, '&') != 0) {
      $apu = str_replace('&amp;', '&', $linkki);
      return str_replace('&', '&amp;', $apu);
    }
    return $linkki;
  }

  /**
   * Create the page navigation.
   *
   * @param integet $curPage current page
   * @param integer $maxPage number of pages
   * @param string $str source data
   * @return string result XML data
   */
  private function tee_navigointi($curPage, $maxPage, $str) {
    if ($curPage == -1) {
      return "";
    }
    $edSivu = 0;
    $sivu = 0;
    $taso2xml = '';
    $haettu_asetettu = FALSE;
    $alinks = $this->pura_linkit($str);
    if (strpos($str, '...') != 0) {
      $poikki = TRUE;
    } else {
      $poikki = FALSE;
    }
    if ($curPage == 1) {
      $taso2xml = $this->luo_tagi('Valinta', 'Edellinen', array('separator' => "no"));
      $taso3xml = $this->luo_tagi('Haettu', '1');
      $taso2xml .= $this->luo_tagi('Valinta', $taso3xml, array('separator' => "no"));
      $edSivu = 1;
    }
    for ($i = 0; $i < count($alinks[1]); $i++) {
      if (is_numeric($alinks[2][$i])) {
        $sivu = $alinks[2][$i];
      }
      if ((($curPage + 1) == $sivu) && ($curPage != 1) && ($haettu_asetettu == FALSE)) {
        $taso3xml = $this->luo_tagi('Haettu', $curPage);
        $taso2xml .= $this->luo_tagi('Valinta', $taso3xml, array('separator' => "no"));
        $haettu_asetettu = TRUE;
      } elseif (($poikki == TRUE) && (($edSivu + 1) != $sivu) &&
              ($alinks[2][$i] != 'Edellinen') && ($alinks[2][$i] != 'Seuraava')) {
        $taso2xml .= $this->luo_tagi('Valinta', '...&nbsp;', array('separator' => "no"));
      }
      $taso3xml = $this->luo_linkki($alinks[1][$i], $alinks[2][$i]);
      $taso2xml .= $this->luo_tagi('Valinta', $taso3xml, array('separator' => "no"));
      $edSivu = $sivu;
    }
    if ($curPage == $maxPage) {
      $taso3xml = $this->luo_tagi('Haettu', $maxPage);
      $taso2xml .= $this->luo_tagi('Valinta', $taso3xml, array('separator' => "no"));
      $taso2xml .= $this->luo_tagi('Valinta', 'Seuraava', array('separator' => "no"));
    }
    return $taso2xml;
  }

  /**
   * Removes bold-amrking from received string.
   *
   * @param <type> $str string to examine
   * @return <type> result string
   */
  private function tarkista_haettu($str) {
    $tulos = str_replace('<b>', '', $str);
    return $this->tarkasta_amp(str_replace('</b>', '', $tulos));
  }

  /**
   * Creates navigation to work info page,
   * to browse previous or next work
   *
   * @param array $linkit link list
   * @return string result XML data
   */
  private function luo_selailu_navigointi($linkit) {
    $taso1xml = '';
    $taso3xml = $this->luo_linkki_listalta($linkit, 'Edellinen teos', 2, FALSE);
    if (empty($taso3xml)) {
      $taso2xml = $this->luo_tagi('Valinta', 'Edellinen teos', array("separator" => "no"));
    } else {
      $taso2xml = $this->luo_tagi('Valinta', $taso3xml, array("separator" => "no"));
    }
    $taso2xml .= $this->luo_tagi('Valinta', $this->luo_linkki_listalta($linkit, 'Selauslistalle', 2, FALSE));
    $taso3xml = $this->luo_linkki_listalta($linkit, 'Seuraava teos', 2, FALSE);
    if (empty($taso3xml)) {
      $taso2xml .= $this->luo_tagi('Valinta', 'Seuraava teos');
    } else {
      $taso2xml .= $this->luo_tagi('Valinta', $taso3xml);
    }
    $taso1xml = $this->luo_tagi('Navigointi', $taso2xml);
    return $taso1xml;
  }

  /**
   * Determines table structure in given html page.
   *
   * @param string $homepage source page
   * @return mixed table start and end points or FALSE if fails
   */
  private function maarita_rajat($homepage) {
    $atulos = $btulos = NULL;
    if (preg_match_all("/<table/", $homepage, $atulos, PREG_OFFSET_CAPTURE) !== FALSE) {
      if (preg_match_all("/<\/table>/", $homepage, $btulos, PREG_OFFSET_CAPTURE) !== FALSE) {
        if (count($atulos[0]) <= count($btulos[0])) {
          $tp = array();
          for ($i = 0; $i < count($atulos[0]); $i++) {
            $tp[] = array('alku' => $atulos[0][$i][1]);
            $loppu[] = $btulos[0][$i][1];
          }
          if ($this->pura_kohde($homepage, "/Osan tiedot/") !== FALSE) {
            if ((count($atulos[0]) == (count($btulos[0]) - 1))) {
              // "Osan tiedot" add one more table and
              // one odd table ending. 19.1.2010 MHa
              for ($i = 3; $i < count($loppu); $i++) {
                $loppu[$i] = $btulos[0][$i + 1][1];
              }
            }
          }
          return $this->laske_taulu_rajat($tp, $loppu);
        } else {
          return FALSE;
        }
      } else {
        return FALSE;
      }
    } else {
      return FALSE;
    }
  }

  /**
   * Count the actual start and end of each table.
   *
   * @param array $tables tables starts
   * @param array $btulos tables ends
   * @return array result of starts and ends
   */
  private function laske_taulu_rajat($tables, $btulos) {
    $tables[]['alku'] = $btulos[count($btulos) - 1] + 1;
    $cnt = count($tables);
    for ($i = 0; $i < count($btulos); $i++) {
      for ($ii = 1; $ii < $cnt; $ii++) {
        if ($btulos[$i] < $tables[$ii]['alku']) {
          if (!isset($tables[$ii - 1]['loppu'])) {
            $tables[$ii - 1]['loppu'] = $btulos[$i];  // Muutettu += -> =  6.3.2013 MHa
          } else {
            for ($iii = $ii - 2; $iii >= 0; $iii--) {
              if (!isset($tables[$iii]['loppu'])) {
                $tables[$iii]['loppu'] = $btulos[$i];  // Muutettu += -> =  6.3.2013 MHa
                break;
              }
            }
          }
          break;
        }
      }
    }
    unset($tables[$cnt - 1]);
    return $tables;
  }

  /**
   * Breaks given string in rows if needed.
   *
   * @param string $rivi text to break in rows
   * @return string result XML data
   */
  private function tarkista_rivitys($rivi) {
    if (stripos($rivi, '<br') === FALSE) {
      return $this->luo_tagi("Teksti", $rivi);
    } else {
      $tekstit = preg_split("/(<br \/>)|(<br>)|(<br\/>)/", $rivi);
      $apu = '';
      foreach ($tekstit as $t) {
        $apu .= $this->luo_tagi("Teksti", $t);
      }
      return $apu;
    }
  }

  /**
   * Creates 'selauslista'-table header links.
   *
   * @param array $otsikot where to search
   * @param integer $alku first header column
   * @param integer $loppu last header column
   * @return string result XML data
   */
  private function luo_selaus_otsikot($otsikot, $alku = 1, $loppu = NULL) {
    $taso1xml = '';
    if ($loppu === NULL) {
      $loppu = count($otsikot);
    }
    foreach ($otsikot as $key => $val) {
      if ((($key + 1) < $alku) || (($key + 1) > $loppu)) {
        continue;
      }
      $links = $this->pura_linkit($val);
      if ($links === FALSE) {
        $taso2xml = $this->luo_tagi('Teksti', strip_tags($val));
        $taso2xml .= $this->luo_tagi('Paikka', (string) $key);
      } else {
        $taso2xml = $this->luo_linkki($links[1][0], $links[2][0]);
        $taso2xml .= $this->luo_tagi('Paikka', $key);
      }
      $taso1xml .= $this->luo_tagi('Totsikko', $taso2xml);
    }
    return $taso1xml;
  }

  /**
   * Extract select list options.
   *
   * @param string $kohde where to search
   * @return mixed result array or FALSE if fails
   */
  private function pura_vaihtoehdot($kohde) {
    return $this->pura_kohde($kohde, "/<option.*>(.+)<\/option>/siU");
  }

  /**
   * Extract select list option names.
   *
   * @param string $kohde where to search
   * @return mixed result array or FALSE if fails
   */
  function pura_name($kohde) {
    return $this->pura_kohde($kohde, "/name=(.+)>/siU");
  }

  /**
   * Extract select list option values.
   *
   * @param string $kohde where to search
   * @return mixed result array or FALSE if fails
   */
  function pura_value($kohde) {
    return $this->pura_kohde($kohde, "/value=(.+)>/siU");
  }

  /**
   * Extract select lists.
   *
   * @param string $kohde where to search
   * @return mixed result array or FALSE if fails
   */
  function pura_pudotus_valikko($kohde) {
    return $this->pura_kohde($kohde, "/<select.*>(.+)<\/select>/siU");
  }

  /**
   * Gets select list information from given source.
   *
   * @param string $homepage where to search
   * @return mixed result array or FALSE or NULL if fails or empty
   */
  private function hae_oheistieto($homepage) {
    $otsikko = $this->etsi_otsikko($homepage, 1);
    if ($otsikko !== FALSE) {
      $pudotus = $this->pura_pudotus_valikko($homepage);
      $i = 0;
      foreach ($pudotus[0] as $valikko) {
        $tunnus = $this->pura_name($valikko);
        $valikon_nimi = str_replace('"', '', $tunnus[1][0]);
        $tulos = $this->pura_vaihtoehdot($pudotus[1][$i]);
        if ($tulos !== FALSE) {
          $apu = array();
          foreach ($tulos[0] as $key => $val) {
            $value = $this->pura_value($val);
            $arvoa = str_replace('"', '', $value[1][0]);
            $arvo = str_replace("'", '', $arvoa);
            if (empty($arvo)) {
              $arvo = '-';
            }
            $apu[$arvo] = $tulos[1][$key];
          }
          $pudotus_valikot[$valikon_nimi] = $apu;
        }
        $i++;
      }
    } else {
      $pudotus_valikot = NULL;
    }
    return $pudotus_valikot;
  }

  /**
   * Extract picture marking from given source.
   *
   * @param string $kohde where to search
   * @return mixed result array or FALSE if fails or not found
   */
  private function pura_kuvat($kohde) {
    $ret = array();
    $kuvat = $this->pura_kohde($kohde, "/<img.*>/siU");
    if ($kuvat === FALSE) {
      return FALSE;
    }
    foreach ($kuvat[0] as $val) {
      if (strpos($val, 'Teoksen') !== FALSE) {
        $osat = explode(' ', $val);
        foreach ($osat as $a) {
          if (($pos = strpos($a, 'src')) !== FALSE) {
            $apub = trim(str_replace('src=', ' ', $a));
            $apu = str_replace(
                    array('&', "'", '"'), array('&amp;', '', ''), $apub);
            $ret[] = $apu;
          }
        }
      }
    }
    return $ret;
  }

  /**
   * Gets the availabililty of work from result html-table.
   *
   * @param array $taulukko source table
   * @return string resutl XML data
   */
  private function extract_availability($taulukko, $otsake = '') {
    $taso1xml = '';
    $valiteksti = $this->pura_kohde($taulukko, "/h3>(.+)<table/siU");
    $titles = $this->pura_taulukon_otsikot($taulukko);
    if ($titles !== FALSE) {
      // Availability / Availability in other libraries
      if (strip_tags($titles[0][0]) == 'Kokoelma' &&
              strip_tags($titles[0][1]) == 'Niteitä' &&
              strip_tags($titles[0][2]) == 'Lainattavissa' &&
              strip_tags($titles[0][3]) == 'Tilattu' &&
              strip_tags($titles[0][4]) == 'Eräpäivä') {
        $lkm = count($titles[0]);
        if (($colums = $this->pura_sarakkeet($taulukko)) !== FALSE) {
          $taso2xml = '';
          for ($i = 0; $i < count($colums[1]); $i += $lkm) {
            $linkki = $this->pura_linkit($colums[1][$i]);
            $taso4xml = $this->luo_linkki($linkki[1][0], $linkki[2][0]);
            $taso3xml = $this->luo_tagi('Kokoelma', $taso4xml);
            $taso3xml .= $this->luo_tagi('Niteita', $colums[1][$i + 1]);
            $taso3xml .= $this->luo_tagi('Lainattavissa', $colums[1][$i + 2]);
            $taso3xml .= $this->luo_tagi('Tilattu', $colums[1][$i + 3]);
            $taso3xml .= $this->luo_tagi('Erapaiva', $colums[1][$i + 4]);
            $taso2xml .= $this->luo_tagi('Teos', $taso3xml);
          }
          if (!empty($otsake)) {
            $taso2xml .= $this->luo_tagi('Valiotsikko', $otsake);
          }
          if (!empty($valiteksti[1][0])) {
            $taso2xml .= $this->luo_tagi('Teksti', $valiteksti[1][0]);
          }
          $taso1xml = $this->luo_tagi('Saatavuus', $taso2xml);
        }
      }
    }
    return $taso1xml;
  }

  /**
   * Gets the availabililty of work on chosen library from result html-table.
   *
   * @param array $taulukko source table
   * @return string resutl XML data
   */
  private function extract_chosen_availability($taulukko, $otsake = '') {
    $apu_taulukko = $this->pura_taulukko($taulukko);
    $taulukko = $apu_taulukko[0][0];
    $taso1xml = '';
    if (($titles = $this->pura_taulukon_otsikot($taulukko)) !== FALSE) {
      // Availebility in chosen library
      if (strip_tags($titles[0][0]) == 'Sijainti' &&
              strip_tags($titles[0][1]) == 'Hylly' &&
              strip_tags($titles[0][2]) == 'Luokka' &&
              strip_tags($titles[0][3]) == 'Niteitä' &&
              strip_tags($titles[0][4]) == 'Lainattavissa' &&
              strip_tags($titles[0][5]) == 'Tilattu' &&
              strip_tags($titles[0][6]) == 'Eräpäivä') {
        $lkm = count($titles[0]) + 1;
        if (($colums = $this->pura_sarakkeet($taulukko)) !== FALSE) {
          $taso2xml = '';
          $i = 0;
          do {
            $taso3xml = $this->luo_tagi('Kirjastonimi', $colums[1][$i]);
            for ($ii = $i + 1; $ii < count($colums[1]); $ii += $lkm) {
              if ($colums[1][$ii + 1] == '&nbsp;') {
                break;
              }
              $taso4xml = $this->luo_tagi('Osasto', $colums[1][$ii + 1]);
              $taso4xml .= $this->luo_tagi('Hylly', $colums[1][$ii + 2]);
              $taso4xml .= $this->luo_tagi('Luokka', $colums[1][$ii + 3]);
              $taso4xml .= $this->luo_tagi('Niteita', $colums[1][$ii + 4]);
              $taso4xml .= $this->luo_tagi('Lainattavissa', $colums[1][$ii + 5]);
              $taso4xml .= $this->luo_tagi('Tilattu', $colums[1][$ii + 6]);
              $taso4xml .= $this->luo_tagi('Erapaiva', $colums[1][$ii + 7]);
              $taso3xml .= $this->luo_tagi('Sijainti', $taso4xml);
            }
            $taso2xml .= $this->luo_tagi('Kirjasto', $taso3xml);
            $i = $ii;
          } while ($i < count($colums[1]));
          if (!empty($otsake)) {
            $taso2xml .= $this->luo_tagi('Valiotsikko', $otsake);
          }
          $taso1xml = $this->luo_tagi('Valitun_saatavuus', $taso2xml);
        }
      }
    }
    return $taso1xml;
  }

  /**
   * Extract the information from works second table.
   *
   * @param <type> $tp table ranges
   * @param <type> $homepage page to extract
   * @return string result XML data
   */
  private function extract_inner2($tp, $homepage) {
    $taso2xml = '';
    $taso1xml = '';
    $tulos = substr($homepage, $tp[1]['loppu'], $tp[2]['alku'] - $tp[1]['loppu'] + 8);
    $otsikko = $this->etsi_otsikko($tulos, 3);
    if ($otsikko == 'Osan tiedot') {
      $tulos = substr($homepage, $tp[2]['alku'], $tp[2]['loppu'] - $tp[2]['alku'] + 8);
      if (($colums = $this->pura_sarakkeet($tulos)) !== FALSE) {
        $taso2xml .= $this->muodosta_teos_tiedot($colums);
      }
      $taso1xml = $this->luo_tagi('Osantiedot', $taso2xml);
    } elseif ($otsikko == 'Sisältyy teokseen') {
      $tulos = substr($homepage, $tp[2]['alku'], $tp[2]['loppu'] - $tp[2]['alku'] + 8);
      if (($colums = $this->pura_sarakkeet($tulos)) !== FALSE) {
        $taso2xml .= $this->muodosta_teos_tiedot($colums);
      }
      $taso1xml = $this->luo_tagi('Sisaltyy_teokseen', $taso2xml);
    }
    return $taso1xml;
  }

  /**
   * Extracts additional text after link if present.
   *
   * @param string $link link to examine
   * @return mixed additional string or NULL
   */
  private function find_addional_text($link) {
    $str = preg_split("/<a(.+)<\/a>/", $link);
    if (count($str) < 2) {
      return NULL;
    }
    $my_str = trim($str[1]);
    if (count($str) == 2 && !empty($my_str)) {
      return $my_str;
    }
    return NULL;
  }

  /**
   * Forms from column data XML presentation of work data.
   *
   * @param string $colums table colum data
   * @return string result XML data
   */
  private function muodosta_teos_tiedot($colums) {
    $taso1xml = '';
    for ($i = 0; $i < count($colums[1]) - 1; $i += 2) {
      // Filter possible links from the end of table.
      // In that case colums are combined.
      if (strpos($colums[0][$i], 'colspan=') !== FALSE) {
        continue;
      }
      if (($linkki = ($this->pura_linkit($colums[1][$i + 1]))) === FALSE) {
        $apub = $this->tarkista_rivitys($colums[1][$i + 1]);
        $apu = $this->tarkista_haettu($apub);
        $taso2xml = $this->luo_tagi('Selite', $colums[1][$i]);
        $taso2xml .= $this->luo_tagi('Tieto', $apu);
        $taso1xml .= $this->luo_tagi('Rivi', $taso2xml);
      } else { // Link
        $taso2xml = $this->luo_tagi('Selite', $colums[1][$i]);
        $apu = NULL;
        $rivit = preg_split("/(<br \/>)|(<br>)|(<br\/>)|(, )/", $colums[1][$i + 1]);
        for ($ii = 0; $ii < count($linkki[0]); $ii++) {
          if (count($rivit) == $ii) {
            break;
          }
          $apu = $this->find_addional_text($rivit[$ii]);
          $taso3xml = $this->luo_linkki($linkki[1][$ii], $linkki[2][$ii], $apu);
          $taso2xml .= $this->luo_tagi('Tieto', $taso3xml);
        }
        $taso1xml .= $this->luo_tagi('Rivi', $taso2xml);
      }
    }
    return $taso1xml;
  }

  /**
   * Create text or link XML-tag, depending on passed value.
   *
   * @param string $value source for XML-tag
   * @return string result XML data
   */
  private function add_link_or_text($value) {
    $taso1xml = '';
    if (($linkki = ($this->pura_linkit($value))) === FALSE) {
      $apu = $this->tarkista_rivitys($value);
      return $this->tarkista_haettu($apu);
    } else { // Link
      $rivit = preg_split("/(<br \/>)|(<br>)|(<br\/>)/", $value);
      for ($ii = 0; $ii < count($linkki[0]); $ii++) {
        $apu = $this->find_addional_text($rivit[$ii]);
        $taso1xml .= $this->luo_linkki($linkki[1][$ii], $linkki[2][$ii], $apu);
      }
      return $taso1xml;
    }
  }

  /**
   * Helper method for writing to file.
   *
   * @param string $tied file name
   * @param string $tieto data to add
   * @param string $moodi mode for operation
   */
  static function write_file($tied, $tieto, $moodi = 'w') {
    //search_from_origo::write_file('solut.xml', '<date>'.date(DATE_RFC822).'</date>');
    //search_from_origo::write_file('solut.xml', print_r($solut, TRUE), 'a');
    $fp = fopen("sites/default/files/libdbsearch/" . $tied, $moodi);
    fwrite($fp, $tieto);
    fwrite($fp, "\n");
    fclose($fp);
  }

}

// End Of Class
?>
