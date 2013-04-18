<?php
// $Id: 

?>
<h1><?php print $title; ?></h1>
<p><?php print $part_info; ?></p>
<?php if(empty($no_results)) { ?>
<table class="lista">
    <tr>
        <th><?php print $list_capt[0]; ?></th>
        <th><?php print $list_capt[1]; ?></th>
        <th><?php print $list_capt[2]; ?></th>
    </tr>
    <?php print $libdbsearch_partworks; ?>
</table>
<div><?php print $navi_back; ?></div>
<div><?php print $navi; ?></div>
<?php } ?>