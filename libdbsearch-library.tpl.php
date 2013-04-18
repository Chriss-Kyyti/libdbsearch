<?php
// $Id:

if(!empty($department)) {
?>
<tr class="odd">
    <td colspan="8"><?php print $l_library; ?></td>
</tr>
<?php foreach($department as $dep) { ?>
    <tr class="even">
        <td style="padding-left:2em;"><?php print $dep['l_location']; ?></td>
        <td><?php print $dep['l_shelf']; ?></td>
        <td><?php print $dep['l_class']; ?></td>
        <td class="keskita"><?php print $dep['l_items']; ?></td>
        <td class="keskita"><?php print $dep['l_borrow']; ?></td>
        <td class="keskita"><?php print $dep['l_ordered']; ?></td>
        <td class="keskita"><?php print $dep['l_due']; ?></td>
    </tr>
<?php 
	}
} 
?>
