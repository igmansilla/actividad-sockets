const net = require('net');
const readline = require('readline');

const client = new net.Socket();
const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

const PORT = 8080; // Asegúrate de que este puerto coincida con el del servidor.
const HOST = '127.0.0.1'; // Asumiendo que el servidor está en la misma máquina.

client.connect(PORT, HOST, () => {
    console.log(`Conectado al servidor en ${HOST}:${PORT}`);
});

client.on('data', (data) => {
    const response = data.toString(); // Convertir el buffer en string
    console.log(`Servidor: ${response}`);

    if (response.includes('Seleccione una opción')) {
            if (opcion === '1' || opcion === '2') {
                client.write(`${opcion}\n`); // Enviar opción al servidor
            } else {
                console.log('Opción no válida.');
                client.write('Invalid\n'); // Informar al servidor que la opción es inválida
            }
    } else if (response.includes('Ingrese la longitud')) {
        rl.question('Ingresa la longitud deseada: ', (longitud) => {
            const mensaje = `${longitud}\n`; // Asegúrate de terminar con un salto de línea
            client.write(mensaje);
        });
    } else {
        // Mostrar la respuesta final y cerrar la conexión
        console.log(`Resultado final: ${response}`);
        rl.close();
        client.end(); // Cerrar la conexión del cliente
    }
});

client.on('close', () => {
    console.log('Conexión cerrada.');
});

client.on('error', (err) => {
    console.error(`Error de conexión: ${err.message}`);
});
