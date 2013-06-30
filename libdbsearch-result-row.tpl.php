<?php
/**
 * @file
 * Default theme implementation for displaying a single search result.
 * 
 * $row_data_x  Row's data units
 * 
 * $zebra       Odd or even row.
 */
?>
<tr class="<?php print $zebra; ?>">
   <td><?php print $row_data_0; ?></td>
   <td><?php print $row_data_1; ?></td>
   <td><?php if(isset($row_data_2)) { print $row_data_2; } ?></td>
   <td><?php if(isset($row_data_3)) { print $row_data_3; } ?></td>
   <td><?php if(isset($row_data_4)) { print $row_data_4; } ?></td>
</tr>