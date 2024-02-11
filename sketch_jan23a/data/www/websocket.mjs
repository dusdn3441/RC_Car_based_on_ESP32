export default class WSClient {
	#gateway = `ws://${window.location.hostname}/ws`
	// #gateway = `ws://192.168.1.103/ws`
	#socket = null

	constructor () {
		this.#socket = new WebSocket(this.#gateway)

		this.#socket.onopen = () => {
			console.log('WS: Connection opened')
		}
		this.#socket.onmessage = (e) => {
			console.log('WS: Message received', e.data)
		}
		this.#socket.onerror = (e) => {
			console.log('WS: Error', e)
		}
		this.#socket.onclose = (e) => {
			console.log('WS: Connection closed', e)
		}
	}

	send (msg) {
		this.#socket.send(msg)
	}
}