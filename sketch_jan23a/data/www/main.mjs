import WSClient from './websocket.mjs'
import { Joystick, EventTypes } from './joystick.mjs'

let DeviceType = 'desktop'

// if ('orientation' in screen && 'lock' in screen.orientation) {
// 	screen.orientation.lock('landscape')
// }
// else {
// 	screen.lockOrientation('landscape')
// }

// 모바일 여부를 가리는 자바스크립트 코드
// https://stackoverflow.com/a/11381730
if (navigator.userAgent.match(/Android/i)
	|| navigator.userAgent.match(/webOS/i)
	|| navigator.userAgent.match(/iPhone/i)
	|| navigator.userAgent.match(/iPad/i)
	|| navigator.userAgent.match(/iPod/i)
	|| navigator.userAgent.match(/BlackBerry/i)
	|| navigator.userAgent.match(/Windows Phone/i)) {
	DeviceType = 'mobile'
}

const wsclient = new WSClient()
const joystick = new Joystick(document.querySelector('.joystick'), {
	device: DeviceType,
	events: {
		startup   : () => wsclient.send(`strup|`),
		startdown : () => wsclient.send(`strdown|`),
		startleft : () => wsclient.send(`strleft|`),
		startright: () => wsclient.send(`strright|`),
		stopup    : () => wsclient.send(`stpup|`),
		stopdown  : () => wsclient.send(`stpdown|`),
		stopleft  : () => wsclient.send(`stpleft|`),
		stopright : () => wsclient.send(`stpright|`)
	}
})

document.querySelectorAll('button').forEach(button => {
	button.addEventListener(EventTypes['leftdown'][DeviceType], event => {
		wsclient.send(`str${button.dataset.keytype}`)
	})
	button.addEventListener(EventTypes['leftup'][DeviceType], event => {
		wsclient.send(`stp${button.dataset.keytype}`)
	})
})