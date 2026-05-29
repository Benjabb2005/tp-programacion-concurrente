
<p align="right">
  <img width="100" height="100" alt="logo_unla" src="https://github.com/user-attachments/assets/1e275c16-bbf2-4517-87a1-ad9440be98de" />
</p>

# Trabajo Práctico Integrador: Sistema logístico
> Proyecto Integrador de sistema de gestión logística para Programación Concurrente UNLa.
>
> Equipo desarrollo: Liberatori Gonzalo, Abbatiello Benjamin, Martinez Martina, Juan Francisco Medina.

## 🚀 Descripción
El sistema gestión logística desarrollado tiene por objetivo gestionar y sincronizar los hilos productores y consumidores para que, a través de herramientas de manejo de concurrencia, puedan procesar las distintas tareas de forma concurrente.

## 📐 Reglas de negocio
* **Prioridad de despacho:** Los paquetes con prioridad "Alta" deben ser transferidos a la cinta transportadora antes que los paquetes de menro prioridad, independientemente de su orden de llegada a la Waiting Queue.
* **Control de flujo y accesos:**
  - La Processing Queue sólo acepta un paquete por vez, debe existir un retardo simulado de 420ms entre asignaciones.
  - No es posible retirar más de un paquete de la Processing Queue simultáneamente. Debe existir un retardo de 270ms entre cada liberación.
  - La producción de paquetes no tiene retardo pero el ingreso de nuevas peticiones a la Waiting Queue es de 90ms.
* **Capacidad y latencia de cómputo: **
  - La ProccesingQueue no puede contener más de 5 paquetes activos.
  - Un paquete debe permanecer en la Processing Queue un mínimo de 550ms antes de que un worker pueda finalizarlo.
* **Alcance de contenidos: ** Los contenidos a utilizar están limitados a los que fueron vistos en la cursada, no se permite el uso de librería opimitivas de sincronización no abordadas.

---

## 🛠️ Requisitos Previos

Para poder abrir y compilar este proyecto se necesita:
* **Code::Blocks IDE** (Se recomienda la versión que viene con el compilador **MinGW / GNU GCC** integrado).
* **Standar Language ** -std=c++11

---

## 💻 Compilación y Ejecución

Seguir estos pasos para correr el proyecto:

### 1️⃣ Clonar el repositorio
Abrir una terminal (consola) en la carpeta donde quiera guardar el proyecto y ejecutar el siguiente comando:
```bash
git clone https://github.com/Benjabb2005/tp-programacion-concurrente.git
```
*(Esto va a descargar una copia del proyecto en tu computadora)*

### 2️⃣ Abrir Code::Blocks

### 4️⃣ Cargar el proyecto
  - En el menú superior, ingresar a: File ➡️ Open... (Archivo ➡️ Abrir).
  - Buscar la carpeta que se descargó y seleccionar el archivo del proyecto: *nombreProyecto.cbp*

### 5️⃣ Buildear y correr
Presionar el botón Build and Run ⚙️ (o tecla F9) paracompilar y ejecutar el programa.  
<br>

> Equipo docente: Lic. Marcos Amaro, Lic. Franco Borsani.
>
> Programación Concurrente - 1° Cuatrimestre 2026
