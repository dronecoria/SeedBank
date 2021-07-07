let status = {};
function ready(fn) {
	if (document.readyState != 'loading') {
		fn();
	} else {
		document.addEventListener('DOMContentLoaded', fn);
	}
}

ready(async function () {

    getStatus()
	setInterval(getStatus, 3 * 1000);


});


async function getStatus() {
	const response = await fetch("/status.json");
	status = await response.json();

	const log = document.querySelector("#log");
	log.innerHTML = JSON.stringify(status);

    printStatus();
}

function printStatus()
{
    const sensors = document.querySelector("#sensors");
    const actuators = document.querySelector("#actuators");
    const others = document.querySelector("#others");
    sensors.innerHTML = "";
    actuators.innerHTML = "";
    others.innerHTML = "";

    const templateOthers = document.querySelector("#other");
    const templateSensor = document.querySelector("#sensor");
    const templateActuator = document.querySelector("#actuator");

    status.sensors.forEach(s => {
        const clone = document.importNode(templateSensor.content, true);

        clone.querySelector(".type").innerHTML = s.type;
        clone.querySelector(".value").innerHTML = s.value;

        sensors.appendChild(clone);
    });

    status.actuators.forEach(a => {
        const clone = document.importNode(templateActuator.content, true);

        clone.querySelector(".name").innerHTML = a.name;
        clone.querySelector(".type").innerHTML = a.type;
        clone.querySelector(".value").innerHTML = a.value;

        actuators.appendChild(clone);
    });

    for (const key in status) {
        if (Object.hasOwnProperty.call(status, key)) {
            if(key != "actuators" && key != "sensors"){

                const clone = document.importNode(templateOthers.content, true);

                clone.querySelector(".name").innerHTML =  key;
                clone.querySelector(".value").innerHTML = status[key];
                others.appendChild(clone);
            }
        }
    }
}
