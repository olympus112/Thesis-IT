N rotaties op voorhand: maak dit een parameter
maak studie: N verhogen verbetert resultaat? Is er een sweet spot?
N kan ook gezien worden als een artistieke parameter: ik wil dat alle patches horizontale lijnen hebben, of dat alle tekst horizontaal is als we krantenpapier gebruiken

Seedpoint initiële grootte hangt af van pixel grootte vs echte grootte van de source --> moet puzzelbaar zijn
Beginnen met de beste bestaande seedpoint te kiezen --> Hoe kiezen we de beste? We laten alle (of maar een aantal) seedpoints even groeien (in meerdere richtingen) en kiezen de best(en) via een priority queue? De groeirichting kunnen we random nemen ipv alle mogelijkheden. Simulated annealing -> cooling effect: random stap verkleint
Zet een limiet op het aantal seedpoints -> als er geen mogelijkheid meer is om een seedpoint te verbeteren:  verplaats de matching limit
De volledige target moet uiteindelijk we gecovered zijn -> hoe verder we coveren, gaan we natuurlijk slechtere matching hebben: we moeten 30% patches niet vergelijken met 70% patches

Vergeet niet dat stukken hout ook een rand hebben: hou rekening dat de source dus al gesneden is

Hoe kiezen we de nieuwe seedpoints? 
- inside out
- edge detection
- possion verdeling -> alle punten liggen even ver van elkaar -> comparison of poission disc distribution paper, houdt wel geen rekening met 
- als er een nieuw punt wordt toegevoegd: 

je kan ook een pregenerated aantal inactive seedpoints hebben, en die pas activeren wanneer je wil, als er een inactive seedpoints overmasked worden, verwijderen we ze