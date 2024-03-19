import re

files = {
    "tex/medusa-MapByLocks.tex": "medusa,maps",
    "tex/medusa-SetByLocks.tex": "medusa,sets",
    "tex/medusa_table.tex": "medusa,locks",
    "tex/mel5-SetByLocks.tex": "mel,sets",
    "tex/melete05.tex": "mel,locks",
    "tex/melete05-MapByLocks.tex": "mel,maps",
    "tex/mike-Locks.tex": "mike,locks",
    "tex/mike-MapByLocks.tex": "mike,maps",
    "tex/mike-SetByLocks.tex": "mike,sets",
    "tex/qbc-MapByLocks.tex": "qbc,maps",
    "tex/qbc-SetByLocks.tex": "qbc,sets",
    "tex/qbc_table.tex": "qbc,locks",
    "tex/skeleton_locks.tex": "skeleton,locks",
    "tex/skeleton_mapByLocks.tex": "skeleton,maps",
    "tex/skeleton_setByLocks.tex": "skeleton,sets",
    "tex/twilight.tex": "twilight,locks",
    "tex/twilight-MapByLocks.tex": "twilight,maps",
    "tex/twilight-SetByLocks.tex": "twilight,sets",
    "tex/buran_locks.tex": "buran,locks",
    "tex/buran_set.tex": "buran,sets",
    "tex/buran_map.tex": "buran,maps"
}

print("machine,bench,lock,result,err")

regex = r"^(?!\\textbf{.+} ?)(?P<lock>.+?)(?:\\HpxLock|\\FairLock)? & (?P<ms>.+) & (?P<err>.+) \\\\$"

for fileName in files.keys():
    with open(fileName, 'r') as file:
        text = file.read()
        matches = re.finditer(regex, text, re.MULTILINE)

        for match in matches:
            print(f"{files[fileName]},{match.group('lock')},{match.group('ms')},{match.group('err')}")

