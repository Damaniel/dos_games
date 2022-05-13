# The game
(name TBD)

## Introduction

The game is styled after traditional roguelikes (like Angband, and, well, Rogue) but with some additional features.

The main quest of the game is to still retreive a special object from the bottom floor of a dungeon and return it to the surface.  However, there are other things to do.

### The main quest

The main quest is very straightforward.  Descend into the dungeon, head to the bottom floor, grab the Holy Vial of Auric Shards and return it back to the surface.  Of course, enemies lurk at every turn, looking to feed on your corpse.  Fortunately, through the power of your gem box and collection of monster cards, in addition to the standard selection of roguelike staples like skills, spells and equipment, you have a fighting chance.  Just remember - like all good roguelikes, death is permanent.

### But what else?

In addition to dungeon diving, why not collect gems and cards, or collect sets of artifacts?  The former two provide tangible benefits to the player, while the latter is really just a measure of how much time you've spent with the game (as the artifacts are saved across all playthroughs, even as characters die).

### But what *else* else?

TBD, really.  

## Mechanics

As this serves as a high level overview, I'm mainly going to concentrate on the parts that aren't general to traditional roguelikes.  Those parts will be described in the related specifications document.

### Gems and the Gem box
Gems serve as the player's primary source of active and passive skills, as well as a way to augment the player's stats.  Each gem comes in one of 4 colors, representing a broad category based on a single stat: red (strength), green (dexterity), blue (intelligence) and yellow (wisdom).  A player's class is largely determined from the composition of gems arranged in their gem box.  The following rules determine the player's class:

- If the gem box is of maximum size, the box is at least 80 percent full of gems of **exactly** 2 colors, and no more than 75 percent of the colored gems in the box are of any one color, the player will become the compound class derived from those two colors as shown below.
- If the gem box is any size and at least 75 percent of the gems slotted in it are of a single color, the player will become the simple class derived from that color as shown below.
- In any other case (primarily where a mix of colors is present in the box but none reach the threshold for one of the simple classes), the player will become an 'Initiate'.  Initiates have the lowest experience requirements, but have relatively stunted stat growth and no class specific abilities.  Additionally, all stat augmentations and skills slotted in the box will have their effectiveness halved.  As such, the goal is to avoid mixing and matching gems of all 3 colors, since the net result is a weaker character overall.  (Note that there are 2 ways to obtain skills that aren't considered 'standard' for a class type - non-optimal color gems and white gems.  See below for more details.)

**Base class:**
- Initiate (any combination that doesn't fall into one of the below categories)

**Simple classes:**
- Warrior (red)
- Ranger (green)
- Wizard (blue)
- Cleric (yellow)

**Compound classes:**
- Thief (red + green)
- Battlemage (red + blue)
- Paladin (red + yellow)
- Ninja (green + blue)
- Druid (green + yellow)
- Dualcaster (blue + yellow)

Both simple and compound classes provide boosts to the base stats they derive from, and offer their own class-specific skills.  Generally, compound classes have higher experience requirements to level, making leveling them a slower process.  Note that if the composition of gems changes in the gembox enough to change the player's class, experience, levels and base stats will be recalculated accordingly - note that this could cause a character to delevel, potentially significantly, depending on circumstances.

Gems can take up more than one space in the box.  As a general rule, the more powerful the effect/spell/skill/etc, the more space it will take.

One key concept regarding gems is that of the 'optimal color' of a gem.  Every skill, spell and augment in the game provided by gems is availble in any color, but each of those has an optimal color - and gems of the optimal color are smaller than those of other colors.  For example, a 'heavy armor' gem has an optimal color of red, being a stereotypically strength-based ability.  As such, the red 'heavy armor' gem is very small compared to the equivalent green, blue and yellow ones.  This design allows any simple or compound class to take on any combination of skills that allow them to keep their class of choice, while allowing the use of skills and abilities that would normally not be availalbe to them.  Note that non-optimal color gems are somewhat more rare than their optimal color versions.  

In addition to color gems, there are also white gems.  These gems provide an individual skill in a single slot form, regardless of the normal size of the colored versions.  These slot into a special part of the gem box, allowing up to 3 such gems to be equipped.  These gems potentially allow any class to use any skill without wasting potentially large amounts of gem box space that would otherwise be needed.  White gems are far more rare than colored ones, but finding one could provide a potentially powerful skill to a class that would otherwise find that skill hard to obtain.

Color gems can be found as monster drops, sitting randomly on the floor of generated dungeons, and are available from shops.  White gems only show up as monster drops, and rarely.  

The gem box is the item that contains all of the player's active gems.  The gem box can be accessed using a keyboard shortcut. The size of the box grows as the player levels, reaching a maximum size somewhere around half the level cap.  (Level cap TBD).  Each of the white gem sockets unlocks approximately 1/3 of the way to the level where the box reaches maximum size. 

### Enemy cards
Enemies in the game have a small chance of dropping enemy cards.  These cards offer boosts that can improve stats, augment existing skills, or add entirely new skills.  A small number of these cards can be active at a time, and collecting a set of related cards can offer a passive bonus regardless of whether the cards are active or not.

The available cards can be browsed in the card collection - empty spaces will show what cards are potentially available (but not what they are), while cards already obtained will show what the card is and what it does.  Collections of related cards can be found in the card collection as well.

An in-town vendor will allow you to sell cards for currency.  Additionally, the player will be able to exchange between 3 and 5 of the same card for a random card of an enemy of roughly the same relative strength (with a very small chance of recieving a card of a higher strength).  Note that the player must still have at least one card remaining after selecting a number of cards to exchange, so in practice the player will need at least 4 copies of a card to do an exchange.

## UI and controls

Like most traditional roguelikes, the UI is entirely text-based.  The main game screen contains a representation of the floor of the dungeon the player is on, a status window, a small window that shows the health of the last damaged enemy, and a text log showing the last few lines of status text.  On EGA systems, this log shows the last 3 lines; on VGA, it shows the last 10.

The primary method of interacting with the game will be the keyboard, but mouse support may be added at some point.

The following screens/menus will be available:
- The main screen (described above).
- The expanded status screen.  This will include all player stats and resistances, plus other miscellaneous stats (like steps taken, time played, etc).
- Inventory.  Shows all non-gem, non-card items and equipment, and allows items to be used and equipped/removed
- The gem box screen.  On this screen, the player can add and remove gems from the box, and show descriptions/shapes of both equipped gems and gems in the inventory.
- The enemy card screen.  On this screen, the player can equip
  cards and look at their card collection/sets.
  
## Detailed specifications
See SPECS.md for details about the various mechanics (gem box sizes, gem types and sizes, stats, items, equipment, etc.)


