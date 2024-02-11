import {
	calculateDiagonal
} from './lib.math.mjs'

const KeyMap = {
	'w': 'up',
	'd': 'right',
	's': 'down',
	'a': 'left',
	'ArrowUp': 'up',
	'ArrowRight': 'right',
	'ArrowDown': 'down',
	'ArrowLeft': 'left',
}
const EventTypes = {
	'leftdown': {
		'desktop': 'mousedown',
		'mobile': 'touchstart',
	},
	'leftmove': {
		'desktop': 'mousemove',
		'mobile': 'touchmove',
	},
	'leftup': {
		'desktop': 'mouseup',
		'mobile': 'touchend',
	}
}

class Joystick {
	//#region Nodes
	#rootNode
	#stickNode
	//#endregion Nodes

	#maxRadius
	#position = {
		x: 0,
		y: 0
	}
	#deviceType
	#events = {
		startup   : () => {},
		startdown : () => {},
		startleft : () => {},
		startright: () => {},
		stopup    : () => {},
		stopdown  : () => {},
		stopleft  : () => {},
		stopright : () => {}
	}
	#isMoving = false
	#direction = 'none'

	constructor (root, params = {}) {
		this.#rootNode = root
		this.#stickNode = root.querySelector('.stick')
		this.#deviceType = params?.device ?? 'desktop'
		this.#events = {
			...this.#events,
			...params?.events
		}

		window.addEventListener('resize', e => {
			const target = e.currentTarget

			this.#setMaxRadius(target)
		})

		this.#setMaxRadius(window)
		this.#bindEvents()
	}

	#setMaxRadius = (target) => {
		this.#maxRadius = Math.min(target.innerWidth, target.innerHeight) / 2
	}
	#calculateMousePosition = (e) => {
		const rect = this.#rootNode.getBoundingClientRect()
		const x = e.clientX - rect.left
		const y = e.clientY - rect.top
		return { x, y }
	}
	#calculateStickPosition = (x, y) => {
		const dx = x - this.#maxRadius
		const dy = y - this.#maxRadius
		const distance = Math.sqrt(dx * dx + dy * dy)
		const angle = Math.atan2(dy, dx)
		const limitedDistance = Math.min(distance, this.#maxRadius)
		const newX = this.#maxRadius + limitedDistance * Math.cos(angle)
		const newY = this.#maxRadius + limitedDistance * Math.sin(angle)
		return { newX, newY }
	}
	#calculateDirection = (x, y) => {
		const dx = x - this.#maxRadius
		const dy = y - this.#maxRadius
		const angle = Math.atan2(dy, dx)

		// Determine the direction based on the angle
		let direction = ''
		if (angle > -Math.PI / 4 && angle <= Math.PI / 4) {
			direction = 'right'
		} else if (angle > Math.PI / 4 && angle <= 3 * Math.PI / 4) {
			direction = 'down'
		} else if (angle > -3 * Math.PI / 4 && angle <= -Math.PI / 4) {
			direction = 'up'
		} else {
			direction = 'left'
		}

		return direction
	}
	#updateStickPosition = (newX, newY) => {
		this.#stickNode.style.left = `${newX}px`
		this.#stickNode.style.top  = `${newY}px`
	}
	#processMovement = (e) => {
		const { x, y } = this.#calculateMousePosition(e)
		const { newX, newY } = this.#calculateStickPosition(x, y)
		this.#direction = this.#calculateDirection(x, y)

		this.#updateStickPosition(newX, newY)

		console.table({ x, y, newX, newY, direction: this.#direction })
	}

	#bindEvents = () => {
		this.#rootNode.addEventListener(EventTypes['leftdown'][this.#deviceType], e => {
			this.#isMoving = true

			this.#processMovement(e)

			this.#events[`start${this.#direction}`]?.()
		})

		this.#rootNode.addEventListener(EventTypes['leftmove'][this.#deviceType], e => {
			if (!this.#isMoving) return
			if (this.#deviceType == 'mobile') { //TODO: dynamic device type
				e.preventDefault()
			}

			this.#processMovement(e)

			this.#events[`start${this.#direction}`]?.()
		})

		this.#rootNode.addEventListener(EventTypes['leftup'][this.#deviceType], e => {
			this.#isMoving = false
			this.#direction = 'none'

			this.#updateStickPosition(this.#maxRadius, this.#maxRadius)

			this.#events[`stopup`]?.()

			console.log(EventTypes['leftup'][this.#deviceType], e, this.#maxRadius)
		})


		window.addEventListener('keydown', event => {
			this.#events[`start${KeyMap[event.key]}`]?.()
		})
		window.addEventListener('keyup', event => {
			this.#events[`stop${KeyMap[event.key]}`]?.()
		})
	}
}

export { Joystick, EventTypes }