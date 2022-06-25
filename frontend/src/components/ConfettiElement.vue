<template>
    <canvas
        id="confetti-canvas"
        ref="confettiCanvas"
    ></canvas>
</template>
<script>
/**
 * Setup a websocket to get a message from the server when an item was scanned.
 * 
 * Once this message is received, some confetti is shown and audio is played.
 */
import confetti from 'canvas-confetti';

export default {
    data() {
        return {
            conf: null,
            socket: null,
            audio: null,
        }
    },
    mounted() {
        this.audio = new Audio(require('../assets/money.wav'));

        this.conf = confetti.create(this.$refs.confettiCanvas, {
            resize: true,
            useWorker: true,
        })

        this.socket = new WebSocket('ws://localhost:3001');

        this.socket.onopen = () => {
            console.log('Socket connected!')
        };

        this.socket.onmessage = (message) => {
            if (message.data === 'new-scan') {
                this.audio.play();
                this.conf({
                    particleCount: 100,
                    spread: 160
                })
            }
        };
    }
}
</script>
<style>
#confetti-canvas {
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    width: 100%;
    height: 100%;
    pointer-events: none;
}
</style>