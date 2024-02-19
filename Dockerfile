FROM ubuntu:latest

# Installer les outils de développement nécessaires
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    libncurses5-dev \
    valgrind

# Définir le répertoire de travail
WORKDIR /usr/src/myapp

# Copier les fichiers du projet dans l'image Docker
COPY . /usr/src/myapp

# Compiler le projet lors de la construction de l'image
RUN make
