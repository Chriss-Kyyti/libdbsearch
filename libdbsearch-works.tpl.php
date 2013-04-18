<?php
// $Id: 

?>
<h1><?php print $title; ?></h1>

<table>
    <tr>
        <td>
            <table>
                <?php print $libdbsearch_works; ?>
            </table>
        </td>
        <td valign="top" align="right">
<?php print $picture.'<br />'.$esittelyteksti; ?>
        </td>
    </tr>
</table>
<div><?php
    print $navi_top.'<br />';
    if(isset($tarkat_tiedot)) {
        print $tarkat_tiedot;
    } else {
        print $saatavuustiedot;
    }
    if(isset($ebook_loan)) {
        print '<br />' . $ebook_loan;
    }    
    if(isset($nayta_sisalto)) {
        print '<br />'.$nayta_sisalto;
    }
    ?></div>
<?php if(isset($kirjastonotsikko)) { ?>
    <br />
    <h2><?php print $kirjastonotsikko; ?></h2>
    <table class="lista">
        <tr>
            <th><?php print t('Location'); ?></th>
            <th class="keskita"><?php print t('Shelf'); ?>&nbsp;</th>
            <th class="keskita"><?php print t('Class'); ?>&nbsp;</th>
            <th class="keskita"><?php print t('Work items'); ?>&nbsp;</th>
            <th class="keskita"><?php print t('Borrowing possible'); ?>&nbsp;</th>
            <th class="keskita"><?php print t('Ordered'); ?>&nbsp;</th>
            <th class="keskita"><?php print t('Due date'); ?></th>
        </tr>
            <?php print $libdbsearch_library; ?>
    </table>
<?php } ?>
<?php if(isset($valiotsikko)) { ?>
<br />
<h2><?php print $valiotsikko; ?></h2>
<p class="tarkennus"><?php print $valiteksti; ?></p>
<table class="lista">
    <tr>
        <th><?php print t('Collection'); ?>&nbsp;</th>
        <th class="keskita"><?php print t('Work items'); ?>&nbsp;</th>
        <th class="keskita"><?php print t('Borrowing possible'); ?>&nbsp;</th>
        <th class="keskita"><?php print t('Ordered'); ?>&nbsp;</th>
        <th class="keskita"><?php print t('Due date'); ?></th>
    </tr>
        <?php print $libdbsearch_avail; ?>
</table>

<?php } ?>
<div><?php print $varaa_teos; ?></div>
<div><?php print $navi_back; ?></div>
<div><?php print $copylink; ?></div>
<div><?php print $vinkki_linkki; ?></div>
<div><?php print $lisaa_node; ?></div>

