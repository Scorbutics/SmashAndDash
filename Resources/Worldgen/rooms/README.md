# Room templates

The rooms of the buildings, one text file per room, read by `worldgen.py` (`rooms.py`). A building gets one of the
rooms of its kind (`house_*.txt`, `center_*.txt`, `rest_*.txt` : the lone rest houses), chosen from the seed.

One character per block; the first two rows are the back wall (`#` its top, `=` its face):

| Char | Block |
|------|-------|
| `#`  | top of the back wall |
| `=`  | face of the back wall |
| `W`  | window (on the wall face) |
| `F`  | framed picture (on the wall face) |
| `.`  | floor |
| `E`  | exit mat (2 wide, last row) : leads back outside |
| `R`  | rug (any rectangle, walkable) |
| `B`  | bed (1 wide, 2 high) ; in a rest house, pressing Action at it heals the team (`Levels/rest.miniska`) |
| `T`  | table (any rectangle) |
| `c`  | stool |
| `S`  | bookshelf (against the wall : its top covers the wall face above) |
| `K`  | kitchen counter (against the wall, any width) |
| `P`  | plant |
| `C`  | healing counter (any rectangle) ; pressing Action in front of it heals the team (`Levels/pokescript.miniska`) |
| `H`  | healing machine (on the healing counter) |

Furniture blocks the player, rugs and mats do not.
