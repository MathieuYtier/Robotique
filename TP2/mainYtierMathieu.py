import math
import random
from math import inf, sqrt
import pygame
from collections import defaultdict

red = (255, 0, 0, 255)
green = (0, 255, 0, 255)
blue = (0, 0, 255, 255)
black = (0, 0, 0, 255)
white = (255, 255, 255, 255)
light_green = (127, 223, 127, 255)
light_blue = (127, 127, 223, 255)

start_color = red
end_color = blue
empty = white
wall = black
visited = light_green
path_color = light_blue


class GridRender:
    def __init__(self, path, zoom):
        self.zoom = zoom
        self.screen = pygame.display.set_mode((100, 100))
        self.picture = pygame.image.load(path).convert_alpha()
        self.picture_width = self.picture.get_width()
        self.picture_height = self.picture.get_height()
        self.screen_width = self.picture_width * self.zoom
        self.screen_height = self.picture_height * self.zoom
        self.screen = pygame.display.set_mode((self.screen_width, self.screen_height))
        self.picture_zoom = pygame.transform.scale(self.picture, (self.screen_width, self.screen_height))

    def set_at(self, i_j, value):
        self.picture.set_at(i_j, value)

    def get_zoom(self):
        return pygame.transform.scale(self.picture, (self.screen_width, self.screen_height))


class Grid:
    def __init__(self, path):
        self.picture = pygame.image.load(path).convert_alpha()
        self.picture_width = self.picture.get_width()
        self.picture_height = self.picture.get_height()

        self.obstacles = []
        self.nodes = []
        self.data = [[0] * self.picture_width] * self.picture_height

        for i in range(self.picture_width):
            for j in range(self.picture_height):
                if self.picture.get_at((i, j)) == white:
                    self.data[i][j] = 1
                    self.nodes.append((i, j))
                if self.picture.get_at((i, j)) == wall:
                    self.obstacles.append((i, j))
                    self.data[i][j] = 0

    def get_nodes(self):
        return self.nodes

    def get_adjacent(self, node):
        i = node[0]
        j = node[1]

        if i < 0 or i >= self.picture_width or j < 0 or j >= self.picture_height:
            raise Exception("Out of bounds")

        if self.data[i][j] != 1:
            return []

        adjacent = []

        for del_pos in [(1, 0), (-1, 0), (0, 1), (0, -1)]:
            new_i = i + del_pos[0]
            new_j = j + del_pos[1]
            if 0 <= new_i < self.picture_width and 0 <= new_j < self.picture_height:
                if self.data[new_i][new_j] == 1:
                    adjacent.append((new_i, new_j))

        return adjacent


def dijkstra(grid, grid_render):
    screen = grid_render.screen
    src = (1, 1)
    dest = (3, 7)
    grid_render.set_at(src, start_color)
    grid_render.set_at(dest, end_color)
    nodes = grid.get_nodes()

    # initialisation des structures de données utiles pour l'algorithme
    continuer = True
    iterations = 0  # compte le nombre d'iteration de la boucle while

    """ Code implémenté """
    predecessors = {node: None for node in nodes}  # Permet de stocker les prédécesseurs des noeuds
    shortest_distance = {node: math.inf for node in nodes}  # On met la distance de chauque noeud à l'infini que dès la première plus petite valeur on le remplace
    shortest_distance[src] = 0  # La distance la plus courte de base est 0 quand on est a source (nous même)

    """---------------------------------------------------------------"""
    # Algorithme
    while nodes and continuer:

        # partie affichage graphique pour suivre l'algorithme
        iterations += 1
        screen.blit(grid_render.get_zoom(), (0, 0))
        # appuyer sur une touche pour quitter
        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN:
                continuer = False
        pygame.time.delay(100)
        pygame.display.flip()
        # fin partie affichage

        value = min(nodes, key=shortest_distance.__getitem__)  # On récupère la valeur minimum de la liste de nodes avec une clé pour savoir dans quel case de la grille on va se placer et on l'enlève de la liste pour pas la reparcourir
        nodes.remove(value)

        for current_node in grid.get_adjacent(value):  # On parcourt les voisins de case actuelle en le récupérant avec get_adjacent
            if current_node != src:  # On vérifie qu'on ne soit pas sur la case src pour ne pas réécrire par dessus
                if current_node not in grid.obstacles:  # On vérifie si le voisin n'est pas un obstacle sinon c'est perdu D:
                    neighbour = shortest_distance[value] + 1  # On ajoute 1 de valeur au voisin de la case car on est à +1 de la case d'avant
                    grid_render.set_at(current_node, visited)  # On affiche la case comme visitée sur l'affichage pygame (vert clair)
                    if neighbour < shortest_distance[current_node]:  # Si la distance du voisin est plus petite que le chemin le plus court actuel,
                        shortest_distance[current_node] = neighbour  # On prend le voisin comme référence de plus courte distance
                        predecessors[current_node] = value  # Et on met la case dans la liste de prédécesseur pour pouvoir se souvenir du chemin parcouru à la fin
            if current_node == dest:  # Si on arrive à la destination, on vide la liste de noeud pour arrêter l'algorithme
                nodes = []

    route = [dest]  # On crée une liste route qui prend la valeur de destination
    current = dest  # Initialisation pour la boucle while qui suit
    while predecessors[current]:  # On remonte la liste de predecessors qu'on stock dans route pour tracer le chemin
        route.append(predecessors[current])  # On ajoute à la liste route la case actuelle
        current = predecessors[current]  # On change de case pour continuer

    # On remet un affichage propre pour pouvoir tracer le chemin entre src et dest
    grid_render = GridRender("mini-map.pgm", 40)  # On met la map en fond
    grid_render.set_at(src, red)  # On set la source
    grid_render.set_at(dest, blue)  # On set la destination

    for move in route:  # Petite boucle for pour pouvoir tracer le chemin le plus court
        if move != src and move != dest:  # On ne trace pas sur src et dest pour mieux voir
            grid_render.set_at(move, path_color)  # On colorie la case en bleu clair

    print(list(reversed(route)))  # On affiche le chemin à parcourir sur le terminal parceque ça fait joli

    continuer = True
    while continuer:

        screen.blit(grid_render.get_zoom(), (0, 0))

        # appuyer sur une touche pour quitter
        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN:
                continuer = False

        pygame.display.flip()

# fonction heuristique : distance euclidienne
def heuristic_cost_estimate(src, dest):
    return sqrt((dest[0] - src[0]) ** 2 + (dest[1] - src[1]) ** 2)


def astar(grid, grid_render):
    screen = grid_render.screen
    src = (1, 1)
    dest = (3, 7)
    grid_render.set_at(src, start_color)
    grid_render.set_at(dest, end_color)
    nodes = grid.get_nodes()

    # initialisation des structures de données utiles pour l'algorithme
    continuer = True
    iterations = 0  # compte le nombre d'iteration de la boucle while

    """ Code implémenté """
    closed = []  # Liste pour les noeuds à ne plus parcourir
    open = [src]  # Liste des noeuds à parcourir
    predecessors = {node: None for node in nodes}  # Liste pour se souvenir d'où on vient
    g = {node: inf for node in nodes}  # Calcul de l'heuristique g
    f = {node: inf for node in nodes}  # Et calcul de l'heuristique f
    g[src] = 0
    f[src] = heuristic_cost_estimate(src, dest)

    """---------------------------------------------------------------"""
    #Algortihme
    while open and continuer:
        # partie affichage graphique pour suivre l'algorithme
        iterations += 1
        screen.blit(grid_render.get_zoom(), (0, 0))
        # appuyer sur une touche pour quitter
        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN:
                continuer = False
        pygame.time.delay(100)
        pygame.display.flip()
        # fin partie affichage

        value = min(open, key=f.__getitem__)  # On récupère la valeur minimum de la liste de nodes avec une clé pour savoir dans quel case de la grille on va se placer et on l'enlève de la liste pour pas la reparcourir
        if value == dest:  #Si la valeur de cette case est la destination alors on arrête l'algorithme
            break

        open.remove(value)  # On enlève la case sur laquelle on se trouve des noeuds ouverts
        closed.append(value)  # Et on l'ajoute aux fermés pour pas qu'on puisse y repasser

        for current_node in grid.get_adjacent(value):  # On parcourt les voisins de la case actuelle en le récupérant avec get_adjacent
            if current_node != src:  # On vérifie qu'on ne soit pas sur la case src pour ne pas réécrire par dessus
                if current_node not in grid.obstacles:  # On vérifie si le voisin n'est pas un obstacle sinon c'est perdu D:
                    grid_render.set_at(current_node, visited)  # On affiche la case comme visitée sur l'affichage pygame (vert clair)
                    if current_node in closed:  # Si le noeud choisi se trouve dans les fermés on ne fait rien car on l'a déjà parcouru
                        continue

                    if current_node not in open:  # Si le noeud voisin n'est pas dans les ouverts on le met pour pouvoir le parcourir au prochain tour de boucle
                        open.append(current_node)

                    neighbour_g = g[value] + 1  # On ajoute 1 à la valeur g de ce voisin
                    if neighbour_g < g[current_node]:  # Si la distance du voisin est plus petite que le chemin le plus court actuel,
                        g[current_node] = neighbour_g  # On prend la distance du voisin comme référence de plus courte distance
                        f[current_node] = g[current_node] + heuristic_cost_estimate(current_node, dest)  # On recalcule la distance à parcourir pour arriver à la destination avec cette nouvelle référence
                        predecessors[current_node] = value  # Et on met la case actuelle dans la liste de de prédécesseur pour pouvoir se souvenir du chemin parcouru à la fin

    route = [dest]  # On crée une liste route qui prend la valeur de destination
    current = dest  # Initialisation pour la boucle while qui suit
    while predecessors[current]:  # On remonte la liste de predecessors qu'on stock dans route pour tracer le chemin
        route.append(predecessors[current])  # On ajoute à la liste route la case actuelle
        current = predecessors[current]  # On change de case pour continuer

    # On remet un affichage propre pour pouvoir tracer le chemin entre src et dest
    grid_render = GridRender("mini-map.pgm", 40)  # On met la map en fond
    grid_render.set_at(src, red)  # On set la source
    grid_render.set_at(dest, blue)  # On set la destination

    for move in route:  # Petite boucle for pour pouvoir tracer le chemin le plus court
        if move != src and move != dest:  # On ne trace pas sur src et dest pour mieux voir
            grid_render.set_at(move, path_color)  # On colorie la case en bleu clair

    print(list(reversed(route)))  # On affiche le chemin à parcourir sur le terminal parceque ça fait joli


    continuer = True
    while continuer:

        screen.blit(grid_render.get_zoom(), (0, 0))

        # appuyer sur une touche pour quitter
        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN:
                continuer = False

        pygame.display.flip()

if __name__ == '__main__':
    pygame.init()
    grid_render = GridRender("mini-map.pgm", 40)
    grid = Grid("mini-map.pgm")

    # commenter ou decommenter pour lancer Dijkstra ou A*
    #dijkstra(grid, grid_render)
    astar(grid, grid_render)

    pygame.quit()
