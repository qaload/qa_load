package io.yury20.chapter12;

import java.util.Random;

interface Generator<T> {
	T next();
}

public class SatanicTextGenerator implements Generator<String> {

	private final int minChars = 50;
	private final int maxChars = 1000;
	private char[] consonants = "бвгджзйклмнпрстфхчцшщ".toCharArray();
	private char[] vowels = "аеёиоуыэюя".toCharArray();
	private char[] marks = ".!?".toCharArray();
	private Random random = new Random();

	@Override
	public String next() {
		int textLength = minChars + random.nextInt(maxChars - minChars);
		StringBuilder sb = new StringBuilder();
		boolean isCap = true;

		while (textLength > 0) {
			StringBuilder word = generateWord(textLength);
			if(isCap) {
				word.replace(0, 1, word.substring(0, 1).toUpperCase());
				isCap = false;
			}
			sb.append(word);
			if(random.nextInt(3) == 0 && word.length() > 3) {
				sb.deleteCharAt(word.length() - 1).append(marks[random.nextInt(marks.length)]);
				isCap = true;
			}
			sb.append(' ');
			textLength -= word.length() + 1;
		}

		return sb.toString();
	}

	private StringBuilder generateWord(final int textLength) {
		int wordLength = 1 + random.nextInt(10);
		boolean startVowel = random.nextBoolean();
		StringBuilder sb = new StringBuilder();
		if(wordLength > textLength)
			wordLength = textLength;
		for (int i = 0; i < wordLength + 1; i++) {
			if(i % 2 == 0)
				sb.append(consonants[random.nextInt(consonants.length)]); // согласная
			else
				sb.append(vowels[random.nextInt(vowels.length)]); // гласная
		}
		if(startVowel)
			sb.deleteCharAt(0);
		else
			sb.deleteCharAt(wordLength);

		return sb;
	}

	public static void main(String[] args) {
		SatanicTextGenerator stg = new SatanicTextGenerator();
		System.out.println(stg.next());
	}
}

