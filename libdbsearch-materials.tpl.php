<?php
// $Id:

?>
<h1><?php print $page_title; ?></h1>
<div id="hakuinfo"><?php print $hits; ?><br /></div>
<table class="lista">
    <tr>
        <th><?php print t('Material'); ?></th>
        <th class=keskita><?php print t('Found'); ?></th>
    </tr>
    <?php print $libdbsearch_materials; ?>
</table>
<?php print $show_all; ?>