# Réflexions par écrit pour être plus sur de ce que je veux coder

## Mesurer la différence entre la première exécution des kernels et les suivantes

Je veux évaluer la différence entre le temps pris par des exécutions comprenant l'allocation, copie, kernel et free par rapport aux exécutions où l'allocation, copie et free sont mutualisées. Pour se faire, il suffit de prendre les courbes liées à la constante `REPEAT_COUNT_ONLY_PARALLEL` en ignorant la première courbe (qui elle est différente parce que les exécutions lazy y prennent place). Je pourrais aussi faire cette une suite d'exécutions avec allocation, copie, kernel n fois, libération et en associant chaque courbe à son numéro pour en voir l'évolution dans le temps (est-ce qu'au bout de l'itération n on a des temps différents de l'itération n/2 etc.) mais ça nécessite plus de code et du coup c'est plus long à développer.

Deux solutions, donc :

- Comparer la courbe des exécutions avec réallocation/copie systématiques avec la courbe des exécutions avec allocations/copies mutualisées en ignorant le premier lancement.
    - Relativement facile à faire
    - Permet de mesurer les temps en régime établi, lorsque les données sont présentes aux bons endroits
    - Ne permet pas de voir l'évolution du temps pris par le kernel
    - Suppose que les temps en régime établi (une fois les données aux bons endroits) sont à peu près constant et ne sont pas optimisés
    - -> Désactiver temporairement l'écrémage des valeurs aberrantes pour mieux évaluer ce qui se passe, en prenant soin de supprimer la première valeur 

- Faire plusieurs fois le bloc (allocation, copie, (kernel n fois), libération) et associer un numéro à chaque courbe pour voir l'évolution des temps de traitement
    - Intéressant pour voir l'évolution des temps de kernel au fil des exécutions (avec optimisation potentielles au fil du temps)
    - Potentiellement à faire plus tard, nécessite un peu plus de travail que l'autre solution

Dessin des graphiques :  
- Le but serait d'évaluer ce qui est fait en lazy, voir le surcoût qu'on paie la première fois puis le coût en régime établi lorsque les données sont présentes aux bons endroits. (régime établi est un peu un abus de langage ici). Donc je pense que ça serait bien d'avoir la superposition de courbes pour lesquelles ça a un sens de superposer des choses, qui sont un minimum comparables.
- Ca serait top que je réussisse à faire des courbes en pointillé ou en petits traits pour pouvoir clairement discerner les temps en régime établi et en premier lancement.
- Du coup, affichage de chaque courbe à tour de rôle : device 1er lancement, device lancements suivants, shared 1er lancement, ...
- Une idée pour que ce soit plus clair serait d'afficher les autres courbes avec un alpha de 0.7, la courbe précédente (le 1er lancement, si applicable) avec un alpha de 0.85 et la courbe actuelle avec un alpha de 1 pour qu'on puisse mieux voir. *Mais relou l'alpha parce que ça implique de changer pas mal de choses dans mon code...)
- Pour une première approche simple et rapide à développer, je vais tout laisser sur le même graphique, et dessiner en pointillés l'autre courbe.