MasterMFD
=========

MFD that allows the user to sort other MFD's into categories, for easier access.

Use
----
Edit the file OrbiterRoot/Config/MasterMFD/mainTree.txt
Setup the file in a tree structure, with whitespace seperating levels.
It doesn't matter how much whitespace you use, just htat it is consistant.

Here's a simple example tree:
```
General
  Specific
    Map
  Orbit
  Surface
Communications
  Docking
```

In the MFD, white is navigation information/buttons, yellow text is MFDs, and green text is sub-categories.
