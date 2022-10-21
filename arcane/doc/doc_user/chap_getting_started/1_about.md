# Qu'est-ce que %Arcane ? {#arcanedoc_getting_started_about}

[TOC]

%Arcane est un environnement de développement pour les codes de calculs
numériques parallèles. Il prend en charge les aspects architecturaux d'un code
de calcul comme les structures de données pour le maillage, le
parallélisme mais aussi des aspects plus liés à
l'environnement comme la configuration du jeu de données.

Un code de calcul, quel qu'il soit, peut être vu comme un système
prenant certaines valeurs en entrée et fournissant des valeurs en
sortie en effectuant des *opérations*. Comme il est impossible
de pouvoir traiter tous les types de codes de calcul, %Arcane se
restreint aux codes de calcul ayant les propriétés suivantes:

- le déroulement de l'exécution peut être décrit comme une
  répétition d'une suite d'opérations, chaque exécution de la suite
  d'opération étant appelé une *itération*;
- Le domaine de calcul est discrétisé dans l'espace en un ensemble
  d'éléments, le *maillage*. Ce maillage peut être 1D, 2D ou
  3D. Un maillage est constitué d'au plus quatre type d'éléments : les
  noeuds, les arêtes, les faces et les mailles. Les valeurs manipulées
  par le code reposent sur un de ces types d'éléments.

Chacun des termes décrit précédemment possède une terminologie propre
à %Arcane:

- une opération du code est appelée un [point d'entrée](\ref arcanedoc_core_types_axl_entrypoint).
  La description de la suite d'opération s'appelle
  la [boucle en temps](\ref arcanedoc_core_types_timeloop)
- les valeurs manipulées sont appelées des [variables](\ref arcanedoc_core_types_axl_variable).
  Par exemple, la température, la pression sont des variables.

En général, un code de calcul peut être décomposé en plusieurs parties
distinctes. Par exemple, le calcul numérique proprement dit et la
partie effectuant des sorties pour le dépouillement. De même, un code
peut utiliser plusieurs physiques : hydrodynamique, thermique, ...
Pour assurer la modularité d'un code, %Arcane fournit ce qu'on appelle
[un module](\ref arcanedoc_core_types_module) qui regroupe l'ensemble des points
d'entrée et variables correspondant à une partie donnée du code.

Enfin, les modules ont souvent besoin de capitaliser certaines
fonctionnalités. Par exemple, un module thermique et un module hydrodynamique
peuvent vouloir utiliser le même schéma numérique. Pour assurer
la capitalisation du code, %Arcane fournit ce qu'on appelle
[un service](\ref arcanedoc_core_types_service)

Les 4 notions décrites précédemment (point d'entrée, variable, module et
service) sont les notions de base de %Arcane. Elles sont décrites plus en
détail dans le document \ref arcanedoc_core_types. Néanmoins, avant
de voir plus en détail le fonctionnement des ces trois objets,
il faut connaître les notions de base présentées dans les chapitres
de ce document.

\note %Arcane est implémenté en utilisant le langage C++ norme 2020.
Même si des fonctionnalités avancées du C++ sont utilisés en
interne, les types et opérations mises à la disposition du développeur
numériciens ont soigneusement été étudiées pour rester le plus simple
possible.


____

<div class="section_buttons">
<span class="back_section_button">
\ref arcanedoc_getting_started
</span>
<span class="next_section_button">
\ref arcanedoc_getting_started_basicstruct
</span>
</div>