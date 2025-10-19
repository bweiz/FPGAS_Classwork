# Code quality grading rubric

<table><thead>
  <tr>
    <th colspan="2">Criterion</th>
    <th colspan="2">Missing or incomplete</th>
    <th colspan="2">Many issues</th>
    <th colspan="2">Minor issues</th>
    <th colspan="2">Met expectations</th>
  </tr></thead>
<tbody>
  <tr>
    <td>Description</td>
    <td>Points</td>
    <td>Description</td>
    <td>Points</td>
    <td>Description</td>
    <td>Points</td>
    <td>Description</td>
    <td>Points</td>
    <td>Description</td>
    <td>Points</td>
  </tr>
  <tr>
    <td><b>Code formatting</b></td>
    <td>3</td>
    <td>Formatting is inconsistent and makes the code hard to read</td>
    <td>0</td>
    <td>Formatting is inconsistent but readable; formatting may be generally inconsistent between files.</td>
    <td>1</td>
    <td>Formatting is mostly consistent, save for a few minor inconsistencies.</td>
    <td>2</td>
    <td>Code formatting is consistent and follows the style guide.</td>
    <td>3</td>
  </tr>
  <tr>
    <td><b>Code quality</b></td>
    <td>4</td>
    <td>Code has a large number of things that don't follow best practices (hard-coded values, magic numbers, commented-out or unused code (dead code), duplicated code, inconsistent naming conventions, non-descriptive names, etc.)</td>
    <td>0</td>
    <td>Code has many instances that don't follow best practices; names are generally not descriptive.</td>
    <td>1.5</td>
    <td>Code has a few instances that don't follow best practices; names are mostly descriptive, but could be better</td>
    <td>3</td>
    <td>Code doesn't have any hard-coded values or magic numbers; there is no dead or unnecessarily duplicated code; names are descriptive</td>
    <td>4</td>
  </tr>
  <tr>
    <td><b>Code modularity and organization</b></td>
    <td>3</td>
    <td>Code is poorly organized and not modular; everything is in one file; files are in seemingly random places in the repository.</td>
    <td>0</td>
    <td>Code is somewhat organized, but many files are in unlikely locations; organization could be clearer or more consistent; code is somewhat modular, but many modules do multiple unrelated things (i.e., they don't have a single responsibility); the code seems thrown together rather than designed.</td>
    <td>1</td>
    <td>Code and files are well organized; code is mostly modular, but a few modules do multiple unrelated things (i.e., they don't have a single responsibility).</td>
    <td>2</td>
    <td>Code organization is clear; code is split into modules that have a clear single purpose; modules are loosely coupled to other modules and can easily be reused.</td>
    <td>3</td>
  </tr>
</tbody></table>
