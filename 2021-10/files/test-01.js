import http from 'k6/http';
import { group } from "k6";
import { sleep } from 'k6';

let response;


export let options = {
  scenarios: {
    case1_scenario: {
      exec: 'case1',
      executor: 'ramping-vus',
      startVUs: 100,
      stages: [
        { duration: '10s', target: 100 },
        { duration: '10s', target: 100 },
      ]
    },
    case2_scenario: {
      exec: 'case2',
      executor: 'ramping-vus',
      startVUs: 50,
      stages: [
        { duration: '10s', target: 50 },
        { duration: '10s', target: 50 },
      ]
    }
  }
};

export function case1() {
  group('user flow: returning user', function () {
    group('visit homepage', function () {
      // load homepage resources
	  response=http.get('https://test.k6.io', {
		  tags: { name: 'HomepageResources' },
	  });
      //console.log(response.body);
    });
    group('login', function () {
      // perform login
	  response=http.get('https://test.k6.io/contacts.php', {
		  tags: { name: 'PostsItemURL' },
	  });
      //console.log(response.body);
    });
  });
  
  sleep(5);
}

export function case2() {
  group('user flow: case2', function () {
    group('news', function () {
      // load news
	  response=http.get('https://test.k6.io/news.php', {
		  tags: { name: 'News' },
	  });
      //console.log(response.body);
    });
    group('coin', function () {
      // perform coin
	  response=http.get('https://test.k6.io/flip_coin.php', {
		  tags: { name: 'Coin1' },
	  });
      //console.log(response.body);
	  response=http.get('https://test.k6.io/flip_coin.php', {
		  tags: { name: 'Coin2' },
	  });
      //console.log(response.body);
    });
  });
  
  sleep(5);
}