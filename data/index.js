function ready(fn) {
	if (document.readyState != 'loading') {
		fn();
	} else {
		document.addEventListener('DOMContentLoaded', fn);
	}
}

ready(function () {

	setInterval(getAverageTemp, 1 * 1000);

});


async function getAverageTemp() {
	let response = await fetch("/avgtemp");
	this.averageTemp = await response.json();

	const output = document.querySelector("#avgTemp");
	output.innerHTML = this.averageTemp;
}
