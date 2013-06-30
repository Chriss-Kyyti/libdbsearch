<?php

/*
 * @file
 * Availability in city / municipality libraries.
 * 
 * $subheader   Header text for table
 * $subtext     Description / help for table
 * $theader_x   Table row titles
 * $avail_rows  Availability rows data
 */
?>

<h3><?php print $subheader; ?></h3>
<?php if (isset($subtext)) { ?>
  <p><?php print $subtext; ?></p>
<?php } ?>
<table>
 <thead>
  <tr>
   <th><?php print $theader_0; ?></th>
   <th><?php print $theader_1; ?></th>
   <th><?php print $theader_2; ?></th>
   <th><?php print $theader_3; ?></th>
   <th><?php print $theader_4; ?></th>
  </tr>
 </thead>
 <tbody><?php print $avail_libs_rows; ?></tbody>
</table>
