# Press Maj+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import math
import random
import matplotlib.pyplot as plt


###############################
# World Class
# Déplacement du robot
###############################
class World:

    def __init__(self, size, position):
        self.size = size
        self.point = [0.0] * size
        self.door = []
        self.door_size = 50
        self.sensor_size = 50
        self.scan = [0.0] * self.sensor_size
        self.robot_position = position
        self.weight_of_doors = [1.0] * size
        self.max_weight = 50
        self.min_weight = 10

    # ajoute une porte dans un monde vide (ne gère pas la collision de porte si plusieurs porte)
    def add_door(self, d):  # d est le centre de la porte
        a = 0
        self.door.append(d)
        half = self.door_size / 2
        d = d - half
        if d < 0:
            d += self.size
        w = 0.0
        while a < 50:
            self.point[math.floor(d)] = 1.0
            a += 1
            if d == self.size:
                d = 0
            else:
                d += 1
            #  weight update
            if a < half:
                w += 1
            else:
                w -= 1
            self.weight_of_doors[math.floor(d)] += w

    # deplace le robot dans le vrai monde : a utiliser dans le main
    def move(self, robot_motion):
        self.robot_position += robot_motion
        if self.robot_position < 0:
            self.robot_position += self.size
        if self.robot_position > self.size:
            self.robot_position -= self.size

    # renvoie un scan en fonction de la position reelle du robot : a utiliser dans le main
    def sense(self):
        # sense doors and non doors (perfect sensor)
        self.scan = [0.0] * world.sensor_size  # empty
        pos = self.robot_position
        half_sensor = self.sensor_size / 2
        i_s = 0
        for i in range(math.floor(pos - half_sensor), math.floor(pos + half_sensor)):
            self.scan[i_s] = self.point[i % self.size]
            i_s += 1
        return self.scan

    # trouve une distance relative à un centre de porte
    def match_scan(self, scan):
        if scan[0] == 1:
            sum = 0
            i = 0
            while (i < self.sensor_size and scan[i] != 0):
                sum += 1
                i += 1
            return self.door_size - sum
        if scan[self.door_size - 1] == 1:
            sum = 0
            i = self.door_size - 1
            while (i >= 0 and scan[i] != 0):
                sum += 1
                i -= 1
            return sum - self.door_size
        return -self.door_size - 1  # en dehors

    # renvoie un tableau vide si le robot n'est pas devant une porte
    # ou un ensemble de positions si il est devant une porte
    def possible_pos(self, scan):
        positions = []
        decalage = self.match_scan(scan)
        if (decalage != -self.door_size - 1):
            for i in self.door:
                positions.append(i + decalage)
        return positions

    # affiche un scan en console
    def print_scan(self):
        print()
        for i in range(self.sensor_size):
            print(int(self.scan[i]), end=' ')

    # fonction appelée par weight : cas de la porte
    def weight_door(self, scan):
        decalage = self.match_scan(scan)
        r_weight = [0.0] * self.size
        for i in range(self.size):
            new_i = i - decalage
            if new_i >= self.size:
                new_i -= self.size
            if new_i < 0:
                new_i += self.size
            r_weight[i] = self.weight_of_doors[new_i]
        return r_weight

    # fonction appelée par weight : cas du couloir
    def weight_no_door(self):
        r_weight = [0.0] * self.size
        for i in range(self.size):
            r_weight[i] = (max(1.0, self.door_size / 2 - self.weight_of_doors[i]))
        return r_weight

    # fonction qui affecte des poids aux différentes positions en fonction du scan
    def weight(self, scan):
        door = False
        for i in scan:
            if i == 1:
                door = True
                break
        r_weight = [0.0] * self.size
        if door:
            r_weight = world.weight_door(scan)
        else:
            r_weight = world.weight_no_door()

        return r_weight

    def print(self):
        # a editer pour ameliorer l'affichage du monde
        # for i in range(self.size):
        #    print(f'{int(self.point[i])}', end=' ')

        print()
        print('robot : ', self.robot_position)
        # for i in range(self.size):
        #    if(i==self.robot_position):
        #        print('R ', end='')
        #    else:
        #        print('  ', end='')
        #
        # print()
        # for i in range(self.size):
        #    print(f'{int(self.weight_of_doors[i])}', end=' ')


###############################
# Belief Class
# Déplacement des particules
###############################
class Belief:

    def __init__(self, size, sample_size):
        self.particles = [0.0] * size  # tableau pour accueillir les particules
        self.size = size  # taille du monde
        self.sample_size = sample_size  # nombre de particules

    def print(self):
        # utiliser stem de matplotlib pour afficher un graphique en "barres"
        plt.stem(self.particles)
        plt.show()
        return 0

    def uniform(self):
        for i in range(self.sample_size):
            self.particles[random.randint(0, self.size - 1)] = 1.0

    def motion_update(self, robot_motion):
        # on crée une copie de la list de particules puis on affecte à l'ancienne liste les valeurs décalées en appliquant un cycle lorsqu'on dépasse le max de la liste
        copyParticles = self.particles
        cycle = 0
        for i in range(self.size):
            self.particles[cycle - robot_motion] = copyParticles[i]
            cycle += 1
        return 0

    def sensor_update(self, scan, world):
        return 0

    def resample(self, scan, world):
        weightedList = world.weight(scan)  #On récupère la liste des poids du scan du monde
        if world.possible_pos(scan):  #Si on est in range d'une porte on applique le poids pour pondérer les particules autour de la porte
            for i in range(self.size):
                if (self.particles[i]*weightedList[i] > 2):  #On met une valeur seuil pour détecter la porte et on applique 1 ou 0 si on en est proche
                    self.particles[i] = 1
                else:
                    self.particles[i] = 0
        else:  #Si on ne détecte pas de porte on relance un scan pour pouvoir se déplacer
            world.sense()
        return 0


## Il manque la partie où l'on se rapproche de la porte la plus proche en prenant en compte les anciennes intérations !
## Pour le moment on ne fait que affiner les portes sauf qu'il ne garde pas en mémoire les "anciennes" portes


if __name__ == '__main__':

    # creation d'un monde
    world = World(1000, 80)
    world.add_door(75)
    world.add_door(200)
    world.add_door(500)

    # creation d'un Belief
    x = Belief(1000, 500)
    x.uniform()  # distribution uniforme sur le monde
    x.print()

    # boucle principale a coder
    for i in range(4):
        world.print()
        scannedWorld = world.sense()
        world.move(10)  #On bouge le robot de 10 dans le monde réel
        x.motion_update(10)  #On bouge les particules de 10
        #scannedWorld = world.sense()  #on scan le monde
        x.resample(scannedWorld, world)  #Puis on resample les particules pour que le robot commence à savoir où il est
        x.print()
