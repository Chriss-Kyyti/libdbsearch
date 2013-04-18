<?php
// $Id:

?>
<h1><?php print $origo_title; ?></h1>
<div id="hakuinfo"><?php print $hit_info; ?><br /></div>
<div><?php print $navi0; ?></div>
<?php if(empty($no_results)) { ?>
<table class="lista">
    <tr>
        <th><?php print $list_capt[1]; ?></th>
        <th><?php print $list_capt[2]; ?></th>
        <th><?php print $list_capt[3]; ?></th>
        <th class="keskita">&nbsp;<?php print $list_capt[4]; ?>&nbsp;</th>
        <th><?php print $list_capt[5]; ?></th>
    </tr>
    <?php print $libdbsearch_browses; ?>
</table>
<div><?php print $navi_back; ?></div>
<div><?php print $navi1; ?></div>
<?php } ?>