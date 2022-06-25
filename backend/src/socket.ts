/**
 * Very basic websocket implementation which is used to let the
 * connected clients know when an item is scanned
 */
import { WebSocketServer, WebSocket } from "ws"

const wss = new WebSocketServer({
    port: process.env.WS_PORT ? Number(process.env.WS_PORT) : 3001
})

wss.on('connection', (ws) => {
    ws.send('Hello!')
});

export default wss

export function sendToAllWebsockets(message: string) {
    wss.clients.forEach(client => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(message)
        }
    })
}