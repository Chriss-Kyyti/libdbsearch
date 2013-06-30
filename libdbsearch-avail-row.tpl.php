<?php

/*
 * @file
 * One row of Work availability in chosen city / municipality library.
 * 
 * $kirjastonimi  Library name in chosen city / municipality.
 * $osasto        Deparment
 * $hylly         Shelf where placed
 * $luokka        Class of the work
 * $niteita       Count of works in library
 * $lainattavissa Count of works to be borrowed
 * $tilattu       Count of ordered works
 * $erapaiva      First due date
 * 
 * $zebra         Odd or even row.
 */
?>
  
  <tr class="<?php print $zebra; ?>">
   <td colspan="7"><?php print $kirjastonimi; ?></td>
  </tr>
  
  <tr class="<?php print $zebra; ?>">
   <td><?php print $osasto; ?></td>
   <td><?php print $hylly; ?></td>
   <td><?php print $luokka; ?></td>
   <td><?php print $niteita; ?></td>
   <td><?php print $lainattavissa; ?></td>
   <td><?php print $tilattu; ?></td>
   <td><?php print $erapaiva; ?></td>
  </tr>
  